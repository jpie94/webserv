/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/22 16:50:10 by jpiech           ###   ########.fr       */
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
Server::Server(std::string & Config) : Webserv(), locations()
{
	size_t it = Config.find("server");
	for(size_t i = 0; i < it; i++)
	{
		if(!isspace(Config[i]))
			throw_error("Error in configuration file: directive is not allowed before server bloc !");
	}
	ExtractBloc(Config, it + 6);
	Config.erase(0, it + 6);
}

void	Server::ExtractBloc(std::string & Config, size_t it)
{
	static bool recursion;
	static std::string location_name;
	std::string key, value;
	size_t	i = it;
	CheckBeforeBracket(Config, i);	
	while (Config[i])
	{
		key = GetConfigKey(Config, i);
		if (key.empty())
			break;	
		if (key == "location" )
		{
			location_name = GetConfigKey(Config, i);
			if(location_name.empty())
				break;
			ExtractLocation(Config, i, recursion);
			continue;
		}
		if (key == "server")
			throw_error("Error in configuration file: server directive is not allowed inside server bloc !");
		value = GetConfigValue(Config, i);
		if (value.empty())
			break;
		if(recursion == false)
			this->config[key] = value;
		else
			this->locations[location_name][key]=value;
	}
	if (Config[i] && Config[i] == '}')
	{
		Config.erase(it, (i - it + 1));
		if (recursion == true)
		{
			location_name.clear();
			recursion = false;
		}
	}
	else
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
}


void	Server::CheckBeforeBracket(std::string Config, size_t & i)
{
	while (std::isspace(Config[i]) && Config[i])
		i++;
	if (Config[i] != '{')
			throw_error("Error in configuration file: directive is not allowed between server bloc and '{'!");
	i++;	
}

std::string		Server::GetConfigKey(std::string Config, size_t & i)
{
	size_t j;

	while (std::isspace(Config[i]) && Config[i])
			i++;
		j = i;
		while (!std::isspace(Config[i]) && Config[i])
		{
			if (Config[i] == '}' || Config[i] == '{' || Config[i] == ';')
				return("");
			i++;
		}
		return(Config.substr(j, (i - j)));
}

std::string		Server::GetConfigValue(std::string Config, size_t & i)
{
	size_t j;
	
	while (std::isspace(Config[i]) && Config[i])
		i++;
	j = i;
	while (Config[i] != ';' && Config[i])
	{
		if (Config[i] == '}' || Config[i] == '{' || Config[i] =='\n')
			return("");
		i++;
	}
	i++;
	/*En fait as besoin de check la position de ;, mais je vais avoir besoin d un check pour savoir si les directives sont connues ou non*/
	// for(size_t k = i; Config[k] != '\n' && Config[k] != '}'; k++)
	// {
	// 	if(!isspace(Config[k]))
	// 		throw_error("Pattern error : ';' found before end of value field !");
	// }
	return(Config.substr(j, (i-j) -1));
}

void	Server::ExtractLocation(std::string & Config, size_t & i, bool & recursion)
{
	if (recursion == true)
			throw_error("Error in configuration file: location directive is not allowed inside location bloc !");
	recursion = true;
	ExtractBloc(Config, i);
}

int	Server::make_listening_socket()
{
	struct addrinfo	hint;
	struct addrinfo	*addr;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	const char * IP;
	std::map<std::string, std::string>::iterator it = config.find("server_name");
	if (it != config.end())
		IP = it->second.c_str();
	else
		IP = "localhost";
	if (getaddrinfo(IP, config["listen"].c_str(), &hint, &addr) != 0)
	{
		// throw_error("getaddrinfo");
		std::cout << "getaddrinfo" << std::endl;
		return (0);
	}
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		std::cout << "socket" << std::endl;
	// 	throw_error("socket");
		return (0);
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		close(socket_fd);
		freeaddrinfo(addr);
		std::cout << "bind" << std::endl;
		// throw_error("bind");
		return (0);
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
	{
		std::cout << "listen" << std::endl;
		// throw_error("listen");
		return (0);
	}
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	this->_fd = socket_fd;
	this->_index = _pfds.size()-1;
	// for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	// 	std::cout << it->first << std::endl;
	std::cout << "socket accept!" << '\n';
	//std::cout << "\npfsd.size()= " << _pfds.size() << ", websocket.size()= " << _web_sockets.size() << '\n';
	return(1);
}

void	Server::add_client_to_pollfds()
{
	int	socket_fd;

	std::cout << "New call to add client to pollfd whit fd server fd :" << this->_fd << std::endl;
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

void Server::printconfig()
{
	std::cout << "***************CONFIGURATION FOR SERVER  FD " << this->_fd << "**************"<< std::endl;
	for (std::map<std::string, std::string>::iterator pconf = config.begin(); pconf != config.end(); pconf++)
		std::cout << pconf->first << " " << pconf->second <<std::endl;
	if(!this->locations.empty())
		std::cout << "***************LOCATIONS**************"<< std::endl;
	for (std::map<std::string, std::map<std::string, std::string> >::iterator ploc = locations.begin(); ploc != locations.end(); ploc++)
	{
		std::cout << "______LOCATION NAME : "<< ploc->first <<std::endl;
		for (std::map<std::string, std::string>::iterator locconf = ploc->second.begin(); locconf != ploc->second.end(); locconf++)
		std::cout << locconf->first << " " << locconf->second <<std::endl;	
	}
}

std::map<std::string, std::string>	Server::getConfig()
{
	return(this->config);
}

void	Server::setPort(std::string port)
{
	this->config["listen"] = port;
}