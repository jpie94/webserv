/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/26 16:10:21 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Server.hpp"

#define HEADERLEN 10
#define BODYLEN 0

class Client : public Server
{
    private :
        int             _count;
	    std::string     _recieved;
    public :
	    /* Canonical Form + Paramtric constructor */
                        Client();
                        Client(int fd, nfds_t index , std::map<std::string, std::string> config, std::map<std::string, std::map<std::string, std::string> > locations);
                        Client(const Client&);
        Client&         operator=(const Client&);
        virtual	        ~Client();
	    /* Member Functions */
        void		    send_answer();
        void		    handle_request();
} ;
#endif