/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/27 16:51:08 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

#define HEADERLEN 10
#define BODYLEN 0

class Client : public Server
{
	protected :
		int		_count;
		std::string	_recieved;
	public :
		/* Canonical Form + Paramtric constructor */
				Client();
				Client(int fd, nfds_t index, Server & serv);
				Client(const Client&);
		Client&		operator=(const Client&);
		virtual		~Client();
		/* Member Functions */
		void		send_answer();
		void		handle_request();
} ;

#endif