/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:56:08 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/12 17:46:54 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// *****************_________CANONICAL______________******************
Client::Client(void) :fd(), index(), count(), recieved()
{
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
		this->index = rhs.index;
		this->fd = rhs.fd;
		this->count = rhs.count;
        this->recieved = rhs.recieved;
    }
    return (*this);
}

// *****************_________MEMBER______________******************
Client::Client(int i, nfds_t j) : count(), recieved()
{
	fd = i;
	index = j;
}

void	Client::handle_request(struct pollfd &pfds)
{
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(pfds.fd, &buffer,BUFSIZ, 0);
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
	std::cout << "recieved= " << recieved << std::endl;
	recieved = recieved + buffer;
	count += bytes_read;
	std::cout << "count : " << count << std::endl;
	std::cout << "\ntaille message recu: " << recieved.size() << '\n';
	std::cout << "[" << pfds.fd << "] Got message:\n" << recieved << '\n';
	if (count == HEADERLEN + BODYLEN)
	{
		pfds.events = POLLOUT;
		count = 0;
	}
}

void	Client::send_answer(struct pollfd &pfds)
{
	const char message[] = "hello client, I want to answer you but I'm to dumb to make a reel HTTP/1.1 answer O_o\n";
	ssize_t msg_len = std::strlen(message);
	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl)) ; 
	size_t sent = send(pfds.fd, message + count, msg_len - count, 0);
	// if (sent < 0)
	// 	throw_error("send");
	count += sent;
	std::cout << "\ntaille message envoye: " << count << '\n';
	if(count == msg_len)
	{
		pfds.events = POLLIN;
		//message.erase();
		count = 0;
	}
}