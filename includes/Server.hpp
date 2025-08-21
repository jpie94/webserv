/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:21:13 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/21 12:26:23 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <string>
#include <map>
#include "Webserv.hpp"

class Client;
class Location;

class Server : public Webserv
{
    private :

        std::map<std::string, std::string> config;
        std::map<std::string, std::map<std::string, std::string> > locations;   
    
    public :

        Server();
        virtual ~Server();
        Server(const Server & srcs);
        Server & operator=(const Server & rhs);

        Server(std::string & Config);
		void					ExtractBloc(std::string & Config, size_t it);
		void					CheckBeforeBracket(std::string Config, size_t & i);
		std::string 			GetConfigKey(std::string Config, size_t & i);
		std::string 			GetConfigValue(std::string Config, size_t & i);
		void     			    ExtractLocation(std::string & Config, size_t & i, bool & recursion);
        void	    make_listening_socket();
        void		add_client_to_pollfds();
        void        printconfig();
};

#endif