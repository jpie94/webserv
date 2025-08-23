/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:49 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/23 14:31:04 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <csignal>
#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include "utils.hpp"

#define IP "127.0.0.1"
#define PORT "8081"

typedef enum	e_socket
{
	UNDEF,
	SERVER,
	CLIENT
}		t_socket;

class WebSocket;

class Webserv
{

	protected:
		static std::vector<struct pollfd>	_pfds;
		static std::vector<WebSocket>		_web_sockets;
	public:
			/* Canonical Form */
							Webserv();
							Webserv(const Webserv& srcs);
		Webserv&				operator=(const Webserv& rhs);
		virtual					~Webserv();
			/* Member Functions */
		void					make_listening_socket();
		void					throw_error(const char*);
		void					runWebserv();
		std::vector<struct pollfd>&		getPfds();
};