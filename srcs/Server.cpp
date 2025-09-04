/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/04 18:13:36 by qsomarri         ###   ########.fr       */
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
		this->_fd = rhs._fd;
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
	std::cout << BOLD << CYAN << "***************CONFIGURATION FOR SERVER  FD " << this->_fd << "**************"<< RESET << std::endl;
	for (std::map<std::string, std::string>::iterator pconf = this->_config.begin(); pconf != _config.end(); pconf++)
		std::cout << BOLD << pconf->first << RESET << " " << GREEN << pconf->second << RESET <<std::endl;
	if(!this->_locations.empty())
		std::cout << BOLD << CYAN << "***************LOCATIONS**************"<< RESET << std::endl;
	for (std::map<std::string, std::map<std::string, std::string> >::iterator ploc = _locations.begin(); ploc != _locations.end(); ploc++)
	{
		std::cout << "______LOCATION NAME : " << BOLD << YELLOW << ploc->first << RESET << std::endl;
		for (std::map<std::string, std::string>::iterator locconf = ploc->second.begin(); locconf != ploc->second.end(); locconf++)
		std::cout << BOLD << locconf->first << RESET << " " << GREEN << locconf->second << RESET << std::endl;	
	}
}