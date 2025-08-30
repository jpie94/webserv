/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:29 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/30 16:39:55 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"

std::vector<struct pollfd> Webserv::_pfds;
std::map<int, Client *> Webserv::_clients;
std::map<int, Server *> Webserv::_servers;

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Webserv::Webserv() : _fd(), _index() {}

Webserv::Webserv(const Webserv &srcs)
{
	*this = srcs;
}

Webserv &Webserv::operator=(Webserv const &rhs)
{
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_index = rhs._index;
	}
	return (*this);
}

Webserv::Webserv(char *FileName) : _fd(), _index()
{
	std::string Config;
	if (FileName)
		Config = ExtractConfig(FileName);
	else
		throw_error("Error in Webserv constructor: No default configuration path set yet !");
	while (Config.find("server") != std::string::npos)
		ServerMaker(Config);
	for (size_t i = 0; i < Config.size(); i++)
	{
		if (!isspace(Config[i]))
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
	}
}

Webserv::~Webserv() {}

/*****************	MEMBER		*******************/
std::string Webserv::ExtractConfig(char *FileName)
{
	std::string Config, line;
	std::ifstream ConfigFile(FileName);
	if (!ConfigFile.is_open())
		throw_error(std::string(std::string("Error in ExtractConfig : ") + FileName + " : " + std::strerror(errno)).c_str());
	while (!ConfigFile.eof())
	{
		std::getline(ConfigFile, line);
		Config += line + "\n";
	}
	ConfigFile.close();
	return (Config);
}

void Webserv::ServerMaker(std::string &Config)
{
	std::vector<Server *> servs;
	Server *tempServer = new Server(Config);
	std::string temp;
	std::map<std::string, std::string> conf = tempServer->getConfig();
	std::map<std::string, std::string>::iterator itPort = conf.find("listen");
	std::vector<std::string> ports;
	if (itPort == conf.end())
		temp = "8080";
	else
		temp = itPort->second;
	std::istringstream iss(temp);
	temp.empty();
	while (iss >> temp)
		ports.push_back(temp);
	std::map<std::string, std::string>::iterator itIP = conf.find("server_name");
	std::vector<std::string> IPs;
	if (itIP == conf.end())
		temp = "localhost";
	else
		temp = itIP->second;
	std::istringstream iss2(temp);
	temp.empty();
	while (iss2 >> temp)
		IPs.push_back(temp);
	while (IPs.size() > 0)
	{
		std::vector<std::string> tempPorts = ports;
		CheckAvailablePorts(*IPs.rbegin(), tempPorts);
		while (tempPorts.size() > 0)
		{
			Server *newServer = new Server(*tempServer);
			newServer->setPort(*tempPorts.rbegin());
			newServer->setIP(*IPs.rbegin());
			tempPorts.pop_back();
			servs.push_back(newServer);
		}
		IPs.pop_back();
	}
	delete tempServer;
	for (size_t i = 0; i < servs.size(); i++)
	{
		if (servs[i]->make_listening_socket())
			_servers[servs[i]->_fd] = servs[i];
		else
			delete servs[i];
	}
}

void Webserv::CheckAvailablePorts(std::string currentIP, std::vector<std::string> &tempPorts)
{
	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second->getConfig()["server_name"] == currentIP)
		{
			if (it->second->getConfig()["listen"] == *tempPorts.rbegin())
			{
				std::cerr << "Error in configuration file : conflicting server name \"" << currentIP << "\" on port " << *tempPorts.rbegin() << ", ignored" << std::endl;
				tempPorts.pop_back();
			}
		}
	}
}

void Webserv::runWebserv()
{
	int status;
	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->second->printconfig();
	while (1)
	{
		status = poll(_pfds.data(), _pfds.size(), 2000);
		if (status < 0)
			throw_error("Error in runWebserv : polling failed !");
		if (status == 0)
		{
			std::cout << "Waiting for connection..." << std::endl;
			continue;
		}
		for (nfds_t j = 0; j < _pfds.size(); ++j)
		{
			if (!(_pfds[j].revents & POLLIN) && !(_pfds[j].revents & POLLOUT))
				continue;
			if ((_pfds[j].revents & POLLIN) && _servers.find(_pfds[j].fd) != _servers.end())
				_servers[_pfds[j].fd]->add_client_to_pollfds();
			else if ((_pfds[j].revents & POLLIN) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->handle_request();
			else if ((_pfds[j].revents & POLLOUT) && _clients.find(_pfds[j].fd) != _clients.end())
				{
					if (_clients[_pfds[j].fd]->send_answer())
						break;
				}
		}
	}
}

void Webserv::erase_client()
{
	std::cout << "fd= " << _pfds[this->_index].fd << std::endl;
	if (close(_pfds[this->_index].fd) < 0)
		throw_error(std::string(std::string("Error in erase_client : close failed : ") + std::strerror(errno)).c_str());
	std::cout << "pfd index= " << (_pfds.begin() + this->_index)->fd << std::endl;
	_pfds.erase(_pfds.begin() + this->_index);
	std::map<int, Client *>::iterator it = _clients.find(this->_fd);
	std::cout << "client= " << it->first << std::endl;
	delete it->second;
	_clients.erase(it);
	setIndex();
}

void Webserv::setIndex()
{
	std::map<int, Server *>::iterator itServer = _servers.begin();
	while (itServer != _servers.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itServer->first == _pfds[i].fd)
			{
				itServer->second->_index = i;
				break;
			}
		}
		itServer++;
	}
	std::map<int, Client *>::iterator itClient = _clients.begin();
	while (itClient != _clients.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itClient->first == _pfds[i].fd)
			{
				itClient->second->_index = i;
				break;
			}
		}
		itClient++;
	}
}

void Webserv::throw_error(const char *msg)
{
	clean_close();
	throw std::runtime_error(msg);
}

void Webserv::clean_close()
{
	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->second)
			delete it->second;
	}
	_servers.clear();
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second)
		{
			it->second->clearClient();
			delete it->second;
		}
	}
	_clients.clear();
	for (unsigned int i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd > 0)
		{
			if (close(_pfds[i].fd) < 0)
				throw std::runtime_error(std::string(std::string("Error in clean_close : close failed : ") + std::strerror(errno)).c_str());
			else
				_pfds[i].fd = -1;
		}
	}
}