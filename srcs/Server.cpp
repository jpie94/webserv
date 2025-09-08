/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:26:15 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/08 17:06:17 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Location.hpp"
  
/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/
Server::Server(): Webserv(), _locations(), _config() {} 

Server::Server(const Server& srcs)
{
	*this = srcs;
}

Server	&Server::operator=(Server const& rhs)
{
	if (this != &rhs)
	{
		this->_config = rhs._config;
		this->_fd = rhs._fd;
		for(std::map<std::string, Location*>::const_iterator it = rhs._locations.begin(); it != rhs._locations.end(); it++)
			this->_locations[it->first] = new Location(*it->second);
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
	try
	{
		ExtractBloc(Config, it + 6);
	}
	catch (std::exception &e) 
	{
		for(std::map<std::string, Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
		{
			if(it->second)
				delete it->second;
		}
		throw_error(e.what());
	}	
	Config.erase(0, it + 6);
}

Server::~Server()
{
    this->_config.clear();
	for(std::map<std::string, Location*>::iterator it = _locations.begin(); it != _locations.end(); it++)
	{
		if(it->second)
			delete it->second;
	}
    this->_locations.clear();
}

/*****************	MEMBER		*******************/

void	Server::ExtractBloc(std::string & Config, size_t it)
{
	std::string key, value, location_name;
	size_t	i = it;
	CheckBeforeBracket(Config, i);	
	while (Config[i])
	{
		key = GetConfigKey(Config, i);
		if (key.empty())
			break;	
		CheckDirective(key, false);
		if (key == "location" )
		{
			location_name = GetConfigKey(Config, i);
			if(location_name.empty())
				break;
			if(_locations.find(location_name) != _locations.end())
				throw_error(std::string("Error in configuration file: location is a duplicate : " + key).c_str());		
			Location temp(location_name);
			temp.ExtractLocBloc(Config, i);
			_locations[location_name] = new Location(temp);
			continue;
		}
		value = GetConfigValue(Config, i);
		if (value.empty())
			break;
		this->_config[key] = value;
	}
	if (Config[i] && Config[i] == '}')
		Config.erase(it, (i - it + 1));		
	else
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
}

void	Server::CheckBeforeBracket(std::string Config, size_t & i)
{
	while (std::isspace(Config[i]) && Config[i])
		i++;
	if (Config[i] != '{')
			throw_error("Error in configuration file: directive is not allowed between bloc start and '{'!");
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
		{
			return("");
		}			
		i++;
	}
	return(Config.substr(j, (i - j)));
}

void	Server::CheckDirective(std::string & key, bool mod)
{
	std::vector <std::string> dirs;
	std::string directives, temp;
	size_t i = 0;
	if (mod == false)
	{
		directives = "listen index server_name error_page client_max_body_size location return root autoindex allowed_methods cgi upload_folder";	
		i = 12;
	}	
	else if (mod == true)
	{
		directives = "error_page index client_max_body_size return root autoindex allowed_methods cgi upload_folder";
		i = 9;
	}
	std::istringstream ss(directives);
	while(dirs.size() < i)
	{
		ss >> temp;
		dirs.push_back(temp);
		temp.clear();
	}
	i = 0;
	while (i < dirs.size())
	{
		if (key == dirs[i])
			break;
		i++;
		if (i == dirs.size())
			throw_error(std::string("Error in configuration file: directive is not allowed : " + key).c_str());			
	}
	if(_config.find(key) != _config.end())
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

std::map<std::string, Location*>	Server::getLocations()
{
	return(this->_locations);
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
	for (std::map<std::string, Location *>::iterator ploc = _locations.begin(); ploc != _locations.end(); ploc++)
	{
		std::cout << "______LOCATION NAME : " << BOLD << YELLOW << ploc->first << RESET << std::endl;
		for (std::map<std::string, std::string>::iterator locconf = ploc->second->_config.begin(); locconf != ploc->second->_config.end(); locconf++)
		std::cout << BOLD << locconf->first << RESET << " " << GREEN << locconf->second << RESET << std::endl;	
	}
}