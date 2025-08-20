/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/20 17:43:34 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

/*****************	CANONICAL	*******************/

Server::Server(): Webserv(), config(), locations() {} 

Server::Server(const Server& srcs)
{
	*this = srcs;
}

Server::~Server()
{
    this->config.clear();
    this->locations.clear();
}

Server	&Server::operator=(Server const& rhs)
{
	if (this != &rhs)
	{
	    this->config = rhs.config;
	    this->locations = rhs.locations;
	}
	return (*this);
}
/*****************	MEMBER		*******************/
Server::Server(int i, nfds_t j) : Webserv()
{
	this->_fd = i;
	this->_index = j;
}

void	Server::add_client_to_pollfds()
{
	int	socket_fd;

	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		throw_error("accept");
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	Client *newClient = new Client(socket_fd, _pfds.size() - 1);
	_clients[socket_fd] = newClient;
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
	// std::cout << "\npfsd.size()= " << _pfds.size() << ", websocket.size()= " << _web_sockets.size() << '\n';

}