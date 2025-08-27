/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:21:13 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/27 16:51:13 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Webserv.hpp"

class Client;

class Server : public Webserv
{
	protected:
		std::map<std::string, std::string>				_config;
		std::map<std::string, std::map<std::string, std::string> >	_locations;
	public :
		/* Canonical Form + Paramtric constructor */
		Server();
		Server(std::string & Config);
		Server(const Server & srcs);
		Server &							operator=(const Server & rhs);
		virtual								~Server(); 
		/* Member Functions */
		void								ExtractBloc(std::string & Config, size_t it);
		void								CheckBeforeBracket(std::string Config, size_t & i);
		std::string							GetConfigKey(std::string Config, size_t & i);
		void								CheckDirective(std::string &key, bool recursion, std::string location_name);
		std::string							GetConfigValue(std::string Config, size_t & i);
		int								make_listening_socket();
		void								add_client_to_pollfds();
		/* Getters & setters*/
		std::map<std::string, std::string>				getConfig();
		void								setPort(std::string port);
		void								setIP(std::string IP);
		void								printconfig();
};

#endif