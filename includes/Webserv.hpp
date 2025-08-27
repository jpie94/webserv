/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:49 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 18:47:47 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP

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
#include <fstream>
#include <map>
#include <sstream>
#include "Utils.hpp"

class Client;
class Server;

class Webserv
{

	protected:
		static std::vector<struct pollfd>	_pfds;
		static std::map<int, Client*>		_clients;
		static std::map<int, Server*>		_servers;
		int					_fd;
		int					_index;
	public:
		/* Canonical Form + Paramtric constructor */
							Webserv();
							Webserv(char *FileName);
							Webserv(const Webserv& srcs);
		Webserv&				operator=(const Webserv& rhs);
		virtual					~Webserv();
		/* Member Functions */
		void					ServerMaker(std::string & Config);
		void					CheckAvailablePorts(std::string currentIP, std::vector<std::string>& tempPorts);
		std::string				ExtractConfig(char *FileName);
		void					throw_error(const char*);
		void					runWebserv();
		void					setIndex();
		void					clean_close();
		void					erase_client();
};

#endif