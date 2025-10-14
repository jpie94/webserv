/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/10/14 12:20:16 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <ctime>
#include "Server.hpp"

class Response;
class Request;
class Location;
class CGI;

class Client : public Server
{
	protected:
		int					_server_fd;
		size_t				_count;
		std::vector<char>	_recieved;
		std::string 		_buff;
		std::string 		_CGIoutput;
		std::time_t			_timeout;
		Request*			_request;
		Response*			_response;
		CGI*				_CGI;
		int					_execComplete;
		int					_pipeReadingComplete;
		int					_pipeWritingComplete;

	public:
		/* Canonical Form + Paramtric constructor */
							Client();
							Client(int fd, nfds_t index, Server &serv);
							Client(const Client &);
		Client				&operator=(const Client &);
		virtual				~Client();
		/* Getters */
		CGI*				getCGI() const;
		/* Member Functions */
		void				send_answer();
		void				handle_request();
		int					clientRecv();
		void				clearClient();
		void				clearCGI();
		void				makeResponse();
		int					checkTimeout();
		void				getCGIoutput();
		void				checkStatusCGI();
		int					parserDispatcher();
		void				resetClient();
};

#endif
