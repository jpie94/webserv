#pragma once

#include <unistd.h>
#include <stdio.h>
#include "Webserv.hpp"

#define BODYLEN 5000
#define HEADERLEN 0

class WebSocket : public Webserv
{
	protected:
		int		_fd;
        	nfds_t		_index;
		t_socket	_type;
		int 		_count;
		std::string 	_recieved;
		// std::string request_line;
		// std::string header;
		// int headerlen;
		// std::string body;
		// int bodylen;
		// std::string message;
		// ssize_t message_len;
	public:
			WebSocket();
			virtual ~WebSocket();
			WebSocket(const WebSocket&);
			WebSocket&	operator=(const WebSocket&);
			WebSocket(int fd, nfds_t index, t_socket type);
		int	getType() const;
		void	add_client_to_pollfds();
		void	erase_from_pollfd(nfds_t &j);
		void	send_answer();
        	void	handle_request();
};