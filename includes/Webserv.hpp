/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:49 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/25 17:05:10 by jpiech           ###   ########.fr       */
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
#include <fstream>
#include <map>
#include <sstream>

class Client;
class Server;

class Webserv
{

	protected:
		static std::vector<struct pollfd> _pfds;
		static std::map<int, Client*> _clients;
		static std::map<int, Server*> _servers;
        int		_fd;
		int		_index;

	public:
			/* Canonical Form */
							Webserv();
							Webserv(const Webserv& srcs);
		Webserv&				operator=(const Webserv& rhs);
		virtual					~Webserv();
			/* Member Functions */
								Webserv(char *FileName);
		void					ServerMaker(std::string & Config);
		void 					CheckAvailablePorts(std::string currentIP, std::vector<std::string>& tempPorts);
		std::string				ExtractConfig(char *FileName);
		void					throw_error(const char*);
		void					runWebserv();
		void					setIndex();
		void 					clean_close();
	    void					erase_client();

};