#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"

 std::vector<struct pollfd>	Webserv::_pfds;
 std::map<int, Client*> Webserv::_clients;
 std::map<int, Server*> Webserv::_servers;

/*****************	CANONICAL	*******************/

Webserv::Webserv()
{
}

Webserv::Webserv(const Webserv& srcs)
{
	*this = srcs;
}

Webserv::~Webserv()
{

}

Webserv	&Webserv::operator=(Webserv const& rhs)
{
	(void)rhs;
	// if (this != &rhs)
	// {
	//     _pfds = rhs._pfds;
	//     this->_web_sockets = rhs._web_sockets;
	// }
	return (*this);
}

/*****************	MEMBER		*******************/

Webserv::Webserv(char *FileName)
{
	std::string Config = ExtractConfig(FileName);
	while(Config.find("server") != std::string::npos)
	{
		try
		{
			Server *newServer = new Server(Config);
			_servers[newServer->_fd] = newServer;
		}
		catch (std::exception &e)
		{
			std::cerr<< e.what()<< std::endl;
			continue;
		}
	}
	for(size_t i = 0; i < Config.size(); i++)
	{
		if(!isspace(Config[i]))
			throw std::invalid_argument(std::string("Error : Something left at end of file"));
	}
}

std::string		Webserv::ExtractConfig(char *FileName)
{
	std::string Config, line;
	std::ifstream	ConfigFile(FileName);
	if (!ConfigFile.is_open())
		throw std::runtime_error(std::string("Could not open file : " + std::string(FileName)));
	while (!ConfigFile.eof())
	{
		std::getline(ConfigFile, line);
		Config += line + "\n";
	}
	ConfigFile.close();
	return(Config);
}

void	Webserv::runWebserv()
{
	int status;
	for(std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		it->second->printconfig();
	while(1)
	{
		status = poll(_pfds.data(), _pfds.size(), 2000);
		if (status < 0)
			throw_error("poll");
		if (status == 0)
		{
			std::cout << "Waiting for connection..." << std::endl; 
			continue;
		}
		for (nfds_t j = 0; j < _pfds.size(); ++j)
		{
			if (!(_pfds[j].revents & POLLIN) && !(_pfds[j].revents & POLLOUT))
				continue;
			std::cout << "Ready for I/O operation" << '\n';
			if ((_pfds[j].revents & POLLIN) && _servers.find(_pfds[j].fd) != _servers.end())
				_servers[_pfds[j].fd]->add_client_to_pollfds();
			else if ((_pfds[j].revents & POLLIN) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->handle_request();
			else if ((_pfds[j].revents & POLLOUT) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->send_answer();
		}
	}
}

void	Webserv::setIndex()
{
	std::map<int, Server*>::iterator itServer = _servers.begin();
	while(itServer != _servers.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itServer->first == _pfds[i].fd)
			{
				itServer->second->_fd = i;
				break;
			}			
		}
		itServer++;
	}
	std::map<int, Client*>::iterator itClient = _clients.begin();
	while(itClient != _clients.end())
	{
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (itClient->first == _pfds[i].fd)
			{
				itClient->second->_fd = i;
				break;
			}			
		}
		itClient++;
	}
}

void	Webserv::throw_error(const char* msg)
{
	std::perror(msg);
	for (unsigned int i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd > 0)
		{
			if (close(_pfds[i].fd) < 0)
				throw_error("close");
			else
				_pfds[i].fd = -1;
		}
	}
	throw std::runtime_error(msg);
}

std::vector<struct pollfd> &	Webserv::getPfds()
{
	return (_pfds);
}

void 	Webserv::cleanAll()
{
	for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if(it->second)
			delete it->second;
	}
	_servers.clear();
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if(it->second)
			delete it->second;
	}
	_clients.clear();
}