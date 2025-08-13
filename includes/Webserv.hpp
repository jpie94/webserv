#pragma once
#include <poll.h>
#include <vector>
#include <string>
#include <exception>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include "Client.hpp"

#define PORT "8080"
#define IP "127.0.0.1"

class Server;

class Webserv
{
    protected:
                static std::vector<struct pollfd> _pfds;
                static std::vector<Server> _socket_list;
                size_t              _serverNumber;
    public:
                Webserv(void); //Canonical
                Webserv(const Webserv & srcs);//Canonical
                Webserv & operator=(const Webserv & rhs);//Canonical
                ~Webserv(void);//Canonical
        void     make_listening_socket();
        void    throw_error(const char*);
        void    runWebserv();
};