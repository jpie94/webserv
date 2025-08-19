#include "Webserv.hpp"
#include "WebSocket.hpp"

std::vector<struct pollfd>	Webserv::_pfds;
std::vector<WebSocket>		Webserv::_web_sockets;

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
	// std::cout << "pfsd.size= " << _pfds.size() << '\n';
	// for (unsigned int i = 0; i < _pfds.size(); ++i)
	//     std::cout << "i= " << i << ", fd= " << _pfds[i].fd << std::endl;
	// for (unsigned int i = 0; i < _pfds.size(); ++i)
	// {
	//     if (_pfds[i].fd > 0)
	//     {
	//         if (close(_pfds[i].fd) < 0)
	//             throw_error("close");
	//         else
	//             _pfds[i].fd = -1;
	//     }
	// }
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
	if(Config.find("Server") == std::string::npos)
		throw std::invalid_argument(std::string("No server bloc found in : " + std::string(FileName)));
	while(Config.size() != 0)
	{
		size_t it = Config.find("Server");
		if(it == std::string::npos)
			Config.clear();
		else
			ExtractBloc(Config, it + 6);
	}
}

void	Webserv::ExtractBloc(std::string Config, size_t it)
{
	std::string key, value;

	size_t i = it;
	size_t j;
	while (std::isspace(Config[i] && Config[i]))
		i++;
	if (Config[i] != '{')
		throw std::invalid_argument(std::string("Patern error in configuration bloc before '{'"));
	while (std::isspace(Config[i] && Config[i]))
		i++;
	j = i;
	while (!std::isspace(Config[i] && Config[i]))
		i++;
	key = Config.substr(j, i);
	
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
		Config += line;
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

std::vector<struct pollfd>&	Webserv::getPfds()
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
	struct pollfd temp = {socket_fd, POLLIN, 0};
	_pfds.push_back(temp);
	WebSocket temp1(socket_fd, (_pfds.size() -1), SERVER);
	_web_sockets.push_back(temp1);
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
			if (_web_sockets[j].getType() == SERVER && (_pfds[j].revents & POLLIN))
				this->_web_sockets[j].add_client_to_pollfds();
			else if (this->_web_sockets[j].getType() == CLIENT && (_pfds[j].revents & POLLIN))
				this->_web_sockets[j].handle_request();
			if (this->_web_sockets[j].getType() == CLIENT && (_pfds[j].revents & POLLOUT))
				this->_web_sockets[j].send_answer();
		}
	}
}
