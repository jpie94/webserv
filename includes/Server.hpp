#pragma once

#include "Webserv.hpp"
#include <unistd.h>

// class Webserv;

typedef enum e_socket
{
	SERVER,
	CLIENT
}	t_socket;

class Server : public Webserv
{
	protected:
		int		_fd;
        	nfds_t		_index;
		t_socket	_type;
	public:
		void	add_client_to_pollfds();
		void	erase_from_pollfd(nfds_t &j);
		int	getType() const;
		void	send_answer();
        	void	handle_request();
};