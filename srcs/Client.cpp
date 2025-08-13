/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:56:08 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/13 15:11:32 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// *****************_________CANONICAL______________******************
Client::Client(void) : Server(), _count(), _recieved()
{
	this->_type = CLIENT;
}

Client::Client(const Client& srcs)
{
	*this = srcs;
}

Client::~Client(void)
{
}

Client &Client::operator=(Client const & rhs)
{
    if (this != &rhs)
    {
		this->_index = rhs._index;
		this->_fd = rhs._fd;
		this->_count = rhs._count;
        this->_recieved = rhs._recieved;
    }
    return (*this);
}

// *****************_________MEMBER______________******************
Client::Client(int i, nfds_t j) : _count(), _recieved()
{
	_fd = i;
	_index = j;
	this->_type = CLIENT;
}

void	Client::handle_request()
{
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_pfds[this->_index].fd, &buffer,BUFSIZ, 0);
	// if (bytes_read < 0)
	// {
	// 	std::cout << "[" << pfds.fd << "] Error: recv, connection closed." << '\n';
	// 	erase_from_pollfd(pfds, j);
	// }
	// if (bytes_read == 0)
	// {
	// 	std::cout << "[" << pfds.fd << "] Client socket closed connection." << '\n';
	// 	erase_from_pollfd(pfds, j);
	// }
	std::cout << "index= " << index << std::endl;
	std::cout << "byte read : " << bytes_read << std::endl;
	std::cout << "_recieved= " << _recieved << std::endl;
	_recieved = _recieved + buffer;
	_count += bytes_read;
	std::cout << "count : " << _count << std::endl;
	std::cout << "\ntaille message recu: " << _recieved.size() << '\n';
	std::cout << "[" << _pfds[_index].fd << "] Got message:\n" << _recieved << '\n';
	if (_count == HEADERLEN + BODYLEN)
	{
		_pfds[_index].events = POLLOUT;
		_count = 0;
	}
}

void	Client::send_answer()
{
	const char message[] = "hello client, I want to answer you but I'm to dumb to make a reel HTTP/1.1 answer O_o\n";
	ssize_t msg_len = std::strlen(message);
	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl)) ; 
	size_t sent = send(this->_pfds[this->_index].fd, message + _count, msg_len - _count, 0);
	// if (sent < 0)
	// 	throw_error("send");
	_count += sent;
	std::cout << "\ntaille message envoye: " << _count << '\n';
	if(_count == msg_len)
	{
		this->_pfds[this->_index].events = POLLIN;
		//message.erase();
		_count = 0;
	}
}