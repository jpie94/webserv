/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:59:58 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/27 16:49:47 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Client::Client() : Server(), _count(), _recieved() {}

Client::Client(const Client& srcs)
{
	*this = srcs;
}

Client&	Client::operator=(Client const & rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_count = rhs._count;
		this->_recieved = rhs._recieved;
	}
	return (*this);
}

Client::Client(int fd, nfds_t index , Server & serv) : Server(serv), _count(), _recieved()
{
	this->_fd = fd;
	this->_index = index; 
	printconfig();
}

Client::~Client() {}

/*****************	MEMBER		*******************/

void	Client::handle_request()
{
	char	buffer[4096];

	std::memset(buffer, 0, sizeof(buffer));
	std::cout << _pfds[this->_index].fd <<std::endl;
	int bytes_read = recv(_pfds[this->_index].fd, &buffer,4096, 0);
	if (bytes_read < 0)
	{
		std::cout << "[" << this->_fd << "] Error: recv, connection closed." << '\n';
		this->erase_client();
		return;
	}
	if (bytes_read == 0)
	{
		std::cout << "[" << this->_fd << "] Client socket closed connection." << '\n';
		this->erase_client();
		return;
	}
	this->_recieved += buffer;
	// std::cout << "index= " << this->_index << std::endl;
	// std::cout << "byte read : " << bytes_read << std::endl;
	// std::cout << "_recieved= " << this->_recieved << std::endl;
	this->_recieved = this->_recieved + buffer;
	this->_count += bytes_read;
	// std::cout << "count : " << this->_count << std::endl;
	// std::cout << "\ntaille message recu: " << this->_recieved.size() << '\n';
	// std::cout << "[" << _pfds[this->_index].fd << "] Got message:\n" << this->_recieved << '\n';
	if (this->_count >= HEADERLEN + BODYLEN)
	{
		Request newRequest(*this);
		newRequest.makeResponse();
		_pfds[this->_index].events = POLLOUT;
		this->_count = 0;
	}
}

void	Client::send_answer()
{
	const char	message[] = "hello client, I want to answer you but I'm to dumb to make a real HTTP/1.1 answer O_o\n";
	ssize_t		msg_len = std::strlen(message);

	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl)) ; 
	size_t sent = send(_pfds[this->_index].fd, message + this->_count, msg_len - this->_count, 0);
	if (sent < 0)
	{
		std::cerr << "[" << this->_index << "] Error: send, connection closed." << '\n';
		this->erase_client();
	}
	this->_count += sent;
	// std::cout << "\ntaille message envoye: " << this->_count << '\n';
	if(this->_count == msg_len)
	{
		_pfds[this->_index].events = POLLIN;
		this->_count = 0;
		this->_recieved = "";
	}
}