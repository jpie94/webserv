#include "Webserv.hpp"
#include "Server.hpp"
#include "Client.hpp"

 std::vector<struct pollfd>	Webserv::_pfds;
 std::map<int, Client*> Webserv::_clients;
 std::map<int, Server*> Webserv::_servers;

/*****************	CANONICAL	*******************/

Webserv::Webserv() :_fd(), _index()
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


Webserv::Webserv(char *FileName): _fd(), _index()
{
	std::string Config;
	if (FileName)
		Config = ExtractConfig(FileName);
	else
		throw_error("Error : No default configuration path set yet !");
	while(Config.find("server") != std::string::npos)
		ServerMaker(Config);
	for(size_t i = 0; i < Config.size(); i++)
	{
		if(!isspace(Config[i]))
			throw_error("Error in configuration file : unexpected end of file, expecting '}'.");
	}
}

std::string		Webserv::ExtractConfig(char *FileName)
{
	std::string Config, line;
	std::ifstream	ConfigFile(FileName);
	if (!ConfigFile.is_open())
			throw_error("");
	while (!ConfigFile.eof())
	{
		std::getline(ConfigFile, line);
		Config += line + "\n";
	}
	ConfigFile.close();
	return(Config);
}

void 	Webserv::ServerMaker(std::string & Config)
{
	std::vector<std::string> ports;
	std::vector<Server*> servs;
	Server *newServer = new Server(Config);
	std::string temp;  
	std::map<std::string, std::string> conf = newServer->getConfig();
	std::map<std::string, std::string>::iterator it = conf.find("listen");
	if(it == conf.end())
		temp = "8080";
	else
		temp = it->second;
	std::istringstream iss(temp);
	temp.empty();
	while (iss >> temp)
		ports.push_back(temp);
	newServer->setPort(*ports.rbegin());
	ports.pop_back();
	servs.push_back(newServer);
	while(ports.size() > 0)
	{
		Server *tempServ = new Server(*newServer);
		tempServ->setPort(*ports.rbegin());
		ports.pop_back();
		servs.push_back(tempServ);
	}
	for(size_t i = 0; i < servs.size(); i++)
	{
		if (servs[i]->make_listening_socket())
			_servers[servs[i]->_fd] = servs[i];
		else
			delete servs[i];
	}
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
			{
				std::cout << "pfds de j " << _pfds[j].fd << std::endl;
				std::cout << "fd du serveur " << _servers[_pfds[j].fd]->_fd << std::endl;
				_servers[_pfds[j].fd]->add_client_to_pollfds();
			}
			else if ((_pfds[j].revents & POLLIN) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->handle_request();
			else if ((_pfds[j].revents & POLLOUT) && _clients.find(_pfds[j].fd) != _clients.end())
				_clients[_pfds[j].fd]->send_answer();
		}
	}
}

void	Webserv::erase_client()
{
	std::cout << "Fd de erase client : " << this->_fd << std::endl;
	std::cout << "Index de erase client : " << this->_index << std::endl;
	if (close(_pfds[this->_index].fd) < 0)
		throw_error("");
	std::cout << "Fd de poll fd a l index souhaite " << _pfds[this->_index].fd << std::endl;
	_pfds.erase(_pfds.begin() + this->_index);
	std::map<int, Client*>::iterator it = _clients.find(this->_fd);
	delete it->second;
	_clients.erase(it);
	setIndex();
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
				itServer->second->_index = i;
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
				itClient->second->_index = i;
				break;
			}			
		}
		itClient++;
	}
}

void	Webserv::throw_error(const char* msg)
{
	clean_close();
	if(msg[0] == '\0')
	{
		msg = std::strerror(errno);
	}
	throw std::runtime_error(msg);
}

void 	Webserv::clean_close()
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
	for (unsigned int i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd > 0)
		{
			if (close(_pfds[i].fd) < 0)
				throw std::runtime_error(strerror(errno));
			else
				_pfds[i].fd = -1;
		}
	}
}