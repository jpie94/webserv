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
#include <cstdio>
#include <iostream>

#define PORT "8080"
#define IP "127.0.0.1"

typedef enum e_socket
{
	UNDEF,
	SERVER,
	CLIENT
}	t_socket;

class WebSocket;

class Webserv
{

    protected:
                std::vector<struct pollfd> _pfds;
                std::vector<WebSocket> _socket_list;
    public:
                Webserv(void); //Canonical
                Webserv(const Webserv & srcs);//Canonical
                Webserv & operator=(const Webserv & rhs);//Canonical
                virtual ~Webserv(void);//Canonical
        // virtual void	send_answer();
        // virtual	void	handle_request();
	// virtual void	add_client_to_pollfds();
        // virtual void	erase_from_pollfd(nfds_t &j);
                void     make_listening_socket();
                void    throw_error(const char*);
                void    runWebserv();
};