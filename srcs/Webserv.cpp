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
	// for (std::map<int, Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
	// {
	// 	if(it->second)
	// 		delete it->second;
	// 	it->second = NULL;
	// }
	// _servers.clear();
	// for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	// {
	// 	if(it->second)
	// 		delete it->second;
	// 	it->second = NULL;
	// }
	// _clients.clear();
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
		size_t it = Config.find("server");
		for(size_t i = 0; i < it; i++)
		{
			if(!isspace(Config[i]))
				throw std::invalid_argument(std::string("Error : Something before server bloc"));
		}
		ExtractBloc(Config, it + 6);
		Config.erase(0, it + 6);
	}
	for(size_t i = 0; i < Config.size(); i++)
		{
			if(!isspace(Config[i]))
				throw std::invalid_argument(std::string("Error : Something left at end of file"));
		}
}

void	Webserv::CheckBeforeBracket(std::string Config, size_t & i)
{
	while (std::isspace(Config[i]) && Config[i])
		i++;
	if (Config[i] != '{')
		throw std::invalid_argument(std::string("Patern error in configuration bloc before '{'"));
	i++;	
}

std::string Webserv::GetConfigKey(std::string Config, size_t & i)
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

std::string Webserv::GetConfigValue(std::string Config, size_t & i)
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

std::string Webserv::ExtractLocation(std::string & Config, size_t & i, bool & recursion)
{
	if (recursion == true)
		throw std::invalid_argument(std::string("Patern error in configuration bloc : location inside location !"));
	std::string location_name = GetConfigKey(Config, i);
		if (location_name.empty())
			return("");
	std::cout << "***********Location name : " << location_name << std::endl;
	recursion = true;
	ExtractBloc(Config, i);
	return(location_name);
}

void	Webserv::ExtractBloc(std::string & Config, size_t it)
{
	static bool recursion;
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
			std::string location_name = ExtractLocation(Config, i, recursion);
			if (location_name.empty())
				break;
			continue;
		}
		if (key == "server")
			throw std::invalid_argument(std::string("Patern error in configuration bloc : server inside server !"));
		value = GetConfigValue(Config, i);
		if (value.empty())
			break;
		std::cout << "|Key = " << key << "|" << "Value = " << value << "|" << std::endl;
	}
	if (Config[i] && Config[i] == '}')
	{
		Config.erase(it, (i - it + 1));
		if (recursion == true)
			recursion = false;
	}
	else
		throw std::invalid_argument(std::string("Patern error in configuration bloc : didn't reach and of bloc with '}'"));
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

void	Webserv::make_listening_socket()
{
	struct addrinfo	hint;
	struct addrinfo	*addr;

	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(IP, PORT, &hint, &addr) != 0)
		throw_error("getaddrinfo");
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		throw_error("socket");
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		freeaddrinfo(addr);
		throw_error("bind");
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
		throw_error("listen");
	struct pollfd newPollfd = {socket_fd, POLLIN, 0};
	_pfds.push_back(newPollfd);
	Server *newServer = new Server(socket_fd, (_pfds.size() -1));
	_servers[socket_fd] = newServer;
	// for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	// 	std::cout << it->first << std::endl;
	std::cout << "socket accept!" << '\n';
	//std::cout << "\npfsd.size()= " << _pfds.size() << ", websocket.size()= " << _web_sockets.size() << '\n';
}

void	Webserv::runWebserv()
{
	int status;

	make_listening_socket(); 
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