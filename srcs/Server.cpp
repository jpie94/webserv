/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/27 15:47:17 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
  
/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/
Server::Server(): Webserv(), _config(), _locations() {} 

Server::Server(const Server& srcs)
{
	*this = srcs;
}

Server	&Server::operator=(Server const& rhs)
{
	if (this != &rhs)
	{
	    this->_config = rhs._config;
	    this->_locations = rhs._locations;
	}
	return (*this);
}

Server::Server(std::string & Config) : Webserv(), _locations()
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

Server::~Server()
{
    this->_config.clear();
    this->_locations.clear();
}

/*****************	MEMBER		*******************/
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
		CheckDirective(key, recursion, location_name);
		if (key == "location" )
		{
			recursion = true;
			location_name = GetConfigKey(Config, i);
			CheckDirective(location_name, recursion, location_name);
			if(location_name.empty())
				break;
			ExtractBloc(Config, i);			
			continue;
		}
		value = GetConfigValue(Config, i);
		if (value.empty())
			break;
		if(recursion == false)
			this->_config[key] = value;
		else
			this->_locations[location_name][key]=value;
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

std::string	Server::GetConfigKey(std::string Config, size_t & i)
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

void	Server::CheckDirective(std::string & key, bool recursion, std::string location_name)
{
	std::string directives;
	if (recursion == false)
		directives = "listen server_name error_page client_max_body_size location return root autoindex allowed_methods cgi_ext cgi_bins upload_folder";
	if (recursion == true)
		directives = "error_page client_max_body_size return root autoindex allowed_methods cgi_ext cgi_bins upload_folder";
	if (directives.find(key) == std::string::npos && location_name != key)
		throw_error(std::string("Error in configuration file: directive is not allowed : " + key).c_str());	
	if(recursion == false && _config.find(key) != _config.end() && key != "cgi")
		throw_error(std::string("Error in configuration file: directive is a duplicate : " + key).c_str());	
	if(recursion == true && _locations.find(key) != _locations.end() && key == location_name)
		throw_error(std::string("Error in configuration file: location is a duplicate : " + key).c_str());			
	if(recursion == true && _locations[location_name].find(key) != _locations[location_name].end() && key != "cgi")
		throw_error(std::string("Error in configuration file: directive is a duplicate : " + key).c_str());			
}

std::string	Server::GetConfigValue(std::string Config, size_t & i)
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
	return(Config.substr(j, (i-j) -1));
}

int	Server::make_listening_socket()
{
	struct addrinfo	hint;
	struct addrinfo	*addr;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(_config["server_name"].c_str(), _config["listen"].c_str(), &hint, &addr) != 0)
		return (std::cerr << "Error in make_listening_socket : Could not resolve " << _config["server_name"] << std::endl, 0);
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		return (std::cerr << "Error in make_listening_socket : Could not create a socket for " << _config["server_name"] << std::endl, 0);
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		close(socket_fd);
		freeaddrinfo(addr);
		return (std::cerr << "Error in make_listening_socket : Could not bind " << _config["server_name"] << " on port " <<  _config["listen"] << " : " << std::strerror(errno) << std::endl, 0);
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
		return (std::cerr << "Error in make_listening_socket : Listen failed on fd " << socket_fd << "for server " << _config["server_name"] << ":" <<  _config["listen"] << " : " << std::strerror(errno) << std::endl, 0);
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	this->_fd = socket_fd;
	this->_index = _pfds.size()-1;
	return(1);
}

void	Server::add_client_to_pollfds()
{
	int	socket_fd;

	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		throw_error("Error in add_client_to_pollfds : accept failure !");
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	Client *newClient = new Client(socket_fd, _pfds.size() - 1, this->_config, this->_locations);
	_clients[socket_fd] = newClient;
}

std::map<std::string, std::string>	Server::getConfig()
{
	return(this->_config);
}

void	Server::setPort(std::string port)
{
	this->_config["listen"] = port;
}

void	Server::setIP(std::string IP)
{
	this->_config["server_name"] = IP;
}

void	Server::printconfig()
{
	std::cout << "***************CONFIGURATION FOR SERVER  FD " << this->_fd << "**************"<< std::endl;
	for (std::map<std::string, std::string>::iterator pconf = _config.begin(); pconf != _config.end(); pconf++)
		std::cout << pconf->first << " " << pconf->second <<std::endl;
	if(!this->_locations.empty())
		std::cout << "***************LOCATIONS**************"<< std::endl;
	for (std::map<std::string, std::map<std::string, std::string> >::iterator ploc = _locations.begin(); ploc != _locations.end(); ploc++)
	{
		std::cout << "______LOCATION NAME : "<< ploc->first <<std::endl;
		for (std::map<std::string, std::string>::iterator locconf = ploc->second.begin(); locconf != ploc->second.end(); locconf++)
		std::cout << locconf->first << " " << locconf->second <<std::endl;	
	}
}