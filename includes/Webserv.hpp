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

// class Client;

class Webserv
{
    protected:
                std::vector<struct pollfd> _pfds;
                std::vector<Client> _client_list;
                std::vector<int>    _server_socket;
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