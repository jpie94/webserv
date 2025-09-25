/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:59:58 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/25 11:27:14 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Response.hpp"
#include "CGI.hpp"

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Client::Client() : Server(),  _server_fd(), _count(), _recieved(), _request(), _response(), _CGI()
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
		this->_config = rhs._config;
		this->_error_pages = rhs._error_pages;
		this->_cgi = rhs._cgi;
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

Client::Client(int fd, nfds_t index, Server &serv) : Server(serv), _count(), _recieved(), _request(), _response(), _CGI()
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
		clearClient();
		erase_client();
		return (1);
	}
	return (0);
}

void	Client::makeResponse()
{
	this->_response = new Response(*this->_request);
	// if (this->_CGI)
	// {
	// 	std::ofstream test (this->_CGI->get_FD_Out());
	// 	std::ifstream lol(this->_CGI->get_FD_Out());
	// }
	// 	this->_response->respon = std::string
	//construire la reponse a partir du CGI, mettre directement le resultat dans la reponse
	
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
		if (this->_request->getCGI() == true)
		{
			if (this->_CGI == NULL)
			{
				this->_CGI = new CGI(*this->_request);
				if (this->_request->getStatus() == "500")
					return(makeResponse());
				this->_buff = _buff.substr(findCRLFCRLF(this->_buff) + 4);
			}
			write(this->_CGI->get_FD_In(), this->_buff.c_str(), this->_buff.size());
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
			if (this->_count >= this->_request->getBodyLen() + this->_request->getHeadersLen() + this->_request->getRequestLineLen())
				makeResponse();
		}
}

void	Client::send_answer()
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
			std::cout << this->_response->getResponseMsg() << std::endl;	
			_pfds[this->_index].events = POLLIN;
			this->_count = 0;
			this->_recieved.clear();
			clearClient();
			this->_timeout = std::time(0);
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
	{
		this->_CGI->clear_CGI();
		delete this->_CGI;
		this->_CGI = 0;	
	}
}