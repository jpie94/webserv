#include "Webserv.hpp"

// *****************_________CANONICAL______________******************
Webserv::Webserv(void) : _pfds(), _client_list(), _server_socket(), _serverNumber(0)
{
}

Webserv::Webserv(const Webserv& srcs)
{
	*this = srcs;
}

Webserv::~Webserv(void)
{
}

Webserv &Webserv::operator=(Webserv const & rhs)
{
    if (this != &rhs)
    {
		this->_pfds.clear();
        this->_pfds = rhs._pfds;
        this->_client_list.clear();
        this->_client_list = rhs._client_list;
        this->_server_socket.clear();
        this->_server_socket = rhs._server_socket;
    }
    return (*this);
}

// *****************_________MEMBER______________******************

void    Webserv::throw_error(const char* msg)
{
    std::perror(msg);
    throw std::runtime_error(msg);
}

void Webserv::make_listening_socket()
{
	struct addrinfo hint;
	struct addrinfo *addr;
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
	this->_pfds.push_back(temp);
}
void    Webserv::runWebserv()
{
    int status;
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
            if (_pfds[j].fd == server_fd && (_pfds[j].revents & POLLIN))
                add_client_to_pollfds(_pfds, server_fd, j, _client_list);
            else if (_pfds[j].fd != server_fd && (_pfds[j].revents & POLLIN))
                _client_list[j-1].handle_request(_pfds[j]);
            if (_pfds[j].fd != server_fd && (_pfds[j].revents & POLLOUT))
                _client_list[j-1].send_answer(_pfds[j]);
        }
    }
}