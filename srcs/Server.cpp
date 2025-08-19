/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/19 11:30:17 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*****************	CANONICAL	*******************/

Server::Server(): config(), locations() {} 

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
