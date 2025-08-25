/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 13:55:52 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/25 18:21:56 by qsomarri         ###   ########.fr       */
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

      int 		    _count;
		  std::string 	_recieved;
    
    public :

                    Client();
        virtual		~Client();
                    Client(const Client&);
        Client&	operator=(const Client&);

      Client(int fd, nfds_t index);

		  void		send_answer();
      void		handle_request();
} ;
#endif