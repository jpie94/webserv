/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/12 15:38:50 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/13 15:08:56 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <iostream>
#include <string>
#include <poll.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "Server.hpp"

#define BODYLEN 5000
#define HEADERLEN 0

class Client : public Server
{
    private :
        int _count;
        std::string _recieved;
        // std::string request_line;
        // std::string header;
        // int headerlen;
        // std::string body;
        // int bodylen;
        // std::string message;
        // ssize_t message_len;
    public:

        Client(void); //Canonical
        Client(const Client & srcs);//Canonical
        Client & operator=(const Client & rhs);//Canonical
        ~Client(void);//Canonical
        
        Client(int i, nfds_t j);   
        void	send_answer();
        void	handle_request();
};
#endif 