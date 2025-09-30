/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:59:58 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/30 16:57:23 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Response.hpp"
#include "CGI.hpp"

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Client::Client() : Server(),  _server_fd(), _count(), _recieved(), _request(), _response(), _CGI(), _execComplete(), _pipeReadingComplete(), _pipeWritingComplete()
{
	this->_timeout = std::time(0);
}

Client::Client(const Client &srcs)
{
	*this = srcs;
}

Client &Client::operator=(Client const &rhs)
{
	if (this != &rhs)
	{
		this->_pipeReadingComplete = rhs._pipeReadingComplete;
		this->_pipeWritingComplete = rhs._pipeWritingComplete;
		this->_execComplete = rhs._execComplete;
		this->_CGI = rhs._CGI;
		this->_buff = rhs._buff;
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgis = rhs._cgis;
		this->_server_fd = rhs._server_fd;
		this->_fd = rhs._fd;
		this->_index = rhs._index;
		this->_count = rhs._count;
		this->_recieved = rhs._recieved;
		this->_request = rhs._request;
		this->_response = rhs._response;
		this->_timeout = rhs._timeout;
	}
	return (*this);
}

Client::Client(int fd, nfds_t index, Server &serv) : Server(serv), _count(), _recieved(), _request(), _response(), _CGI(), _execComplete(), _pipeReadingComplete(), _pipeWritingComplete()
{
	this->_server_fd = this->_fd;
	this->_fd = fd;
	this->_index = index;
	this->_timeout = std::time(0);
	// printconfig();
}

Client::~Client() {}

/*****************	MEMBER		*******************/

int	Client::checkTimeout()
{
	double	diff = std::difftime(std::time(0), this->_timeout);
	if (diff > 30.0)
	{
		std::cout  << BOLD << PURPLE << "Client " << this->_fd << " got timeout !\n" << RESET;
		if (this->_CGI)
		{
				kill(this->_CGI->get_PID(), SIGINT);
				this->clearCGI();
				this->_request->setStatus("504");
				this->_execComplete = 1;
				makeResponse();
		}
		else
		{
			clearClient();
			erase_client();
		}
		return (1);
	}
	return (0);
}

void Client::checkStatusCGI()
{
	int	status, exec = 0;
	if(this->_CGI && !this->_execComplete)
	{
		exec = waitpid(this->_CGI->get_PID(), &status, WNOHANG);
		if (exec != 0)
		{
			if (status != 0)
			{
				this->clearCGI();
				this->_request->setStatus("500");
			}
			this->_execComplete = 1;
			makeResponse();
	}
	}
}
void	Client::makeResponse()
{
	this->_response = new Response(*this->_request);
	if (!this->_CGI)
		this->_response->callMethode();
	_pfds[this->_index].events = POLLOUT;	
	this->_count = 0;
}

int Client::clientRecv()
{
	char buffer[4096];

	std::memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_pfds[this->_index].fd, &buffer, 4096, 0);
	this->_buff = buffer;
	if (bytes_read < 0)
	{
		std::cout << "[" << this->_fd << "] Error: recv, connection closed." << '\n';
		this->clearClient();
		this->erase_client();
		return (1);
	}
	if (bytes_read == 0)
	{
		std::cout << BOLD << PURPLE << "[" << this->_fd << "] Client socket closed connection." << RESET << '\n';
		this->clearClient();
		this->erase_client();
		return (1);
	}
	this->_recieved += buffer;
	this->_count += bytes_read;
	this->_timeout = std::time(0);
	std::cout << "[" << _pfds[this->_index].fd << "] Got message:\n" << this->_recieved << '\n';
	return (0);
}

void Client::handle_request()
{
	if (this->_recieved.size() == 0)
		this->_request = new Request(*this);
	if (this->clientRecv())
		return;
	if (!this->_request)
		return;
	this->_request->setRecieved(this->_recieved);
	if (this->_recieved.size() && findCRLFCRLF(this->_recieved) != std::string::npos)
	{
		if (this->_request->getProtocol() != "HTTP/1.1")
			this->_request->parsRequest();
		if (this->_request->get_isCGI() == true)
		{
			if (this->_CGI == NULL)
			{
				this->_CGI = new CGI(*this->_request);
				this->_buff = _buff.substr(findCRLFCRLF(this->_buff) + 4);
			}
			write(this->_CGI->get_FD_In(), this->_buff.c_str(), this->_buff.size());
			this->_timeout = std::time(0);
		} 
		else 
		{
			std::map<std::string, std::string> headers = this->_request->getHeaders();
			if (headers.find("CONTENT-TYPE") != headers.end() && !std::strncmp(headers["CONTENT-TYPE"].c_str(), "multipart/form-data", 19))
				this->_request->parsMultipart();
			else if (headers.find("CONTENT-LENGTH") != headers.end() && this->_request->getBody().size() < this->_request->getBodyLen())
				this->_request->parsBody();
			else if (headers.find("TRANSFER-ENCODING") != headers.end() && headers["TRANSFER-ENCODING"] == "chunked")
				this->_request->parsChunkedBody();
			}
		}
		if (this->_count >= this->_request->getBodyLen() + this->_request->getHeadersLen() + this->_request->getRequestLineLen())
		{
			if (this->_CGI)
			{
				this->_pipeWritingComplete = 1;
				if (close (this->_CGI->get_FD_In()) < 0)
					throw std::runtime_error(std::string(std::string("Error in handle_request : close _CGI _In failed : ") + std::strerror(errno)).c_str());
			}
			else
				makeResponse();
		}
}

void	Client::getCGIoutput()
{
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int i = read(this->_CGI->get_FD_Out(), buffer, 4096);
	this->_CGIoutput += buffer;
	this->_timeout = std::time(0);
	if (i == 0)
	{
		this->_response->setResponseMsg(this->_CGIoutput);
		this->_pipeReadingComplete = 1;
		if (close (this->_CGI->get_FD_Out()) < 0)
			throw std::runtime_error(std::string(std::string("Error in getCGIoutput : close _CGI _Out failed : ") + std::strerror(errno)).c_str());
		this->clearCGI();
	}
}

void	Client::send_answer()
{
	if (this->_request->get_isCGI() == true)
		getCGIoutput();
	else
	{
		size_t msg_len = this->_response->getResponseMsg().size();
		if (!msg_len)
		{
			_pfds[this->_index].events = POLLIN;
			this->_count = 0;
			this->_recieved.clear();
			clearClient();
			return ((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl));
		}
		size_t sent = send(_pfds[this->_index].fd, this->_response->getResponseMsg().c_str() + this->_count, msg_len - this->_count, 0);
		if (sent < 0)
		{
			std::cerr << "[" << this->_index << "] Error: send, connection closed." << '\n';
			this->erase_client();
		}
		this->_count += sent;
		if (this->_count == msg_len)
		{
			if (this->_request->getHeaders().find("CONNECTION") != this->_request->getHeaders().end()
				&& this->_request->getHeaders()["CONNECTION"] == "close")
			{
				std::cout << "connection close ---> erase client...\n";
				clearClient();
				this->erase_client();
				return;
			}
			else
			{
				_pfds[this->_index].events = POLLIN;
				this->_count = 0;
				this->_recieved.clear();
				clearClient();
				this->_timeout = std::time(0);
			}
		}
	}
}

void Client::clearClient()
{
	if (this->_request)
		delete this->_request;
	this->_request = 0;
	if (this->_response)
		delete this->_response;
	this->_response = 0;
	if (this->_CGI)
		this->clearCGI(); 
}

void	Client::clearCGI()
{
	this->_CGIoutput.clear();
	this->_CGI->clear_CGIenv();
	if(!this->_pipeWritingComplete)
		if (close (this->_CGI->get_FD_In()) < 0)
			throw std::runtime_error(std::string(std::string("Error in clear_CGI : close _In failed : ") + std::strerror(errno)).c_str());
	if(!this->_pipeReadingComplete)	
	{
		if (close (this->_CGI->get_FD_Out()) < 0)
			throw std::runtime_error(std::string(std::string("Error in clear_CGI : close _Out failed : ") + std::strerror(errno)).c_str());
	}
	delete this->_CGI;
	this->_CGI = NULL;	
	this->_execComplete = 0;
	this->_pipeWritingComplete = 0;
	this->_pipeReadingComplete = 0;	
	if(this->_request)
		this->_request->set_isCGIFalse();
}

CGI*	Client::getCGI() const
{
	return(this->_CGI);
}