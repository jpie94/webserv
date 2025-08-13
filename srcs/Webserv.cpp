#include "Webserv.hpp"
#include "WebSocket.hpp"

std::vector<struct pollfd> Webserv::_pfds;
std::vector<WebSocket> Webserv::_web_sockets;

// *****************_________CANONICAL______________******************
Webserv::Webserv(void)
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
    (void)rhs;
    // if (this != &rhs)
    // {
    //     this->_pfds = rhs._pfds;
    //     this->_web_sockets = rhs._web_sockets;
    // }
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
    WebSocket temp1(socket_fd, (this->_pfds.size() -1), SERVER);
	this->_web_sockets.push_back(temp1);
    std::cout << "socket accept!" << "\npfsd.size()= " << _pfds.size() << ", websocket.size()= " << _web_sockets.size() << '\n';
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
            if (_web_sockets[j].getType() == SERVER && (_pfds[j].revents & POLLIN))
                this->_web_sockets[j].add_client_to_pollfds();
            else if (this->_web_sockets[j].getType() == CLIENT && (_pfds[j].revents & POLLIN))
                _web_sockets[j].handle_request();
            if (this->_web_sockets[j].getType() == CLIENT && (_pfds[j].revents & POLLOUT))
                _web_sockets[j].send_answer();
        }
    }
}