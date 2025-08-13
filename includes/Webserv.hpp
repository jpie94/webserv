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

#define PORT "8081"
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
                static std::vector<struct pollfd>      _pfds;
                static std::vector<WebSocket>          _web_sockets;
    public:
                                                Webserv(); //Canonical
                                                Webserv(const Webserv& srcs);//Canonical
                Webserv&                        operator=(const Webserv& rhs);//Canonical
                virtual                         ~Webserv(void);//Canonical
                void                            make_listening_socket();
                void                            throw_error(const char*);
                void                            runWebserv();
                // virtual void	send_answer();
                // virtual	void	handle_request();
                // virtual void	add_client_to_pollfds();
                // virtual void	erase_from_pollfd(nfds_t &j);
};