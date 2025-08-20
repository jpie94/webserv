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
#include <fstream>
#include <map>

#define IP "127.0.0.1"
#define PORT "8082"

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
		std::string				ExtractConfig(char *FileName);
		void					ExtractBloc(std::string & Config, size_t it);
		void					CheckBeforeBracket(std::string Config, size_t & i);
		std::string 			GetConfigKey(std::string Config, size_t & i);
		std::string 			GetConfigValue(std::string Config, size_t & i);
		std::string 			ExtractLocation(std::string & Config, size_t & i, bool & recursion);
		void					make_listening_socket();
		void					throw_error(const char*);
		void					runWebserv();
		void					setIndex();
		std::vector<struct pollfd>&		getPfds();
		std::map<int, Client*>		getClient();
		std::map<int, Server*>		getServer();
};