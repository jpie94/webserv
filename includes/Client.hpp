/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/09/02 14:43:53 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Response;
class Request;

class Client : public Server
{
	protected:
		size_t		_count;
		std::string _recieved;
		Request*	_request;
		Response*	_response;

	public:
		/* Canonical Form + Paramtric constructor */
					Client();
					Client(int fd, nfds_t index, Server &serv);
					Client(const Client &);
		Client		&operator=(const Client &);
		virtual		~Client();
		/* Member Functions */
		void			send_answer();
		void		handle_request();
		int			clientRecv();
		void		clearClient();
		void		makeResponse();
};

#endif