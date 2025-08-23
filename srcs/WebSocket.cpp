/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebSocket.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:35 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/23 14:16:37 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebSocket.hpp"

/*****************	CANONICAL	*******************/

WebSocket::WebSocket() : _fd(), _index(), _type(), _count(), _recieved()
{
}

WebSocket::WebSocket(const WebSocket& srcs)
{
	*this = srcs;
}

WebSocket::~WebSocket()
{
}

WebSocket&	WebSocket::operator=(WebSocket const & rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_index = rhs._index;
		this->_type = rhs._type;
		this->_count = rhs._count;
		this->_recieved = rhs._recieved;
	}
	return (*this);
}

/*****************	MEMBER		*******************/

WebSocket::WebSocket(int i, nfds_t j, t_socket type) : _count(), _recieved()
{
	this->_fd = i;
	this->_index = j;
	this->_type = type;
}

void	WebSocket::add_client_to_pollfds()
{
	int	socket_fd;

	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		Webserv::throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	_pfds.push_back(temp);
	WebSocket temp1(socket_fd, (_pfds.size() -1), CLIENT);
	_web_sockets.push_back(temp1);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
	// std::cout << "\npfsd.size()= " << _pfds.size() << ", websocket.size()= " << _web_sockets.size() << '\n';

}

void	WebSocket::erase_client(nfds_t &j)
{
	if (close(_pfds[j].fd) < 0)
		throw_error("close");
	_pfds.erase(_pfds.begin() + j);
	_web_sockets.erase(_web_sockets.begin() + j);
	for(unsigned int i = j; i < _web_sockets.size(); ++i)
		_web_sockets[i]._index--;
}

int	WebSocket::getType() const
{
	return (this->_type);
}


void	WebSocket::handle_request()
{
	char	buffer[4096];

	std::memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_pfds[this->_index].fd, &buffer,BUFSIZ, 0);
	if (bytes_read < 0)
	{
		std::cout << "[" << this->_fd << "] Error: recv, connection closed." << '\n';
		erase_client(this->_index);
		return;
	}
	if (bytes_read == 0)
	{
		std::cout << "[" << this->_fd << "] Client socket closed connection." << '\n';
		erase_client(this->_index);
		return;
	}
	std::cout << "index= " << this->_index << std::endl;
	std::cout << "byte read : " << bytes_read << std::endl;
	std::cout << "_recieved= " << this->_recieved << std::endl;
	this->_recieved = this->_recieved + buffer;
	this->_count += bytes_read;
	std::cout << "count : " << this->_count << std::endl;
	std::cout << "\ntaille message recu: " << this->_recieved.size() << '\n';
	std::cout << "[" << _pfds[this->_index].fd << "] Got message:\n" << this->_recieved << '\n';
	if (this->_count >= HEADERLEN + BODYLEN)
	{
		_pfds[this->_index].events = POLLOUT;
		this->_count = 0;
	}
}

void	WebSocket::send_answer()
{
	const char	message[] = "hello client, I want to answer you but I'm to dumb to make a real HTTP/1.1 answer O_o\n";
	ssize_t		msg_len = std::strlen(message);

	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl)) ; 
	size_t sent = send(_pfds[this->_index].fd, message + this->_count, msg_len - this->_count, 0);
	if (sent < 0)
	{
		std::cerr << "[" << this->_index << "] Error: send, connection closed." << '\n';
		erase_client(this->_index);
	}
	this->_count += sent;
	std::cout << "\ntaille message envoye: " << this->_count << '\n';
	if(this->_count == msg_len)
	{
		_pfds[this->_index].events = POLLIN;
		this->_count = 0;
	}
}