#pragma once

#include <stdio.h>
#include <unistd.h>
#include "Webserv.hpp"

#define BODYLEN 10
#define HEADERLEN 0

class WebSocket : public Webserv
{
	private:
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
		/* Canonical Form*/
				WebSocket();
		virtual		~WebSocket();
				WebSocket(const WebSocket&);
		WebSocket&	operator=(const WebSocket&);
		/* Parametric Constructor */
				WebSocket(int fd, nfds_t index, t_socket type);
		/* Getter */
		int		getType() const;
		/* Member Functions */
		void		add_client_to_pollfds();
		void		erase_client(nfds_t &j);
		void		send_answer();
        	void		handle_request();
};