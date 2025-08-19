/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 11:21:13 by jpiech            #+#    #+#             */
/*   Updated: 2025/08/19 11:30:33 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <string>
#include <map>

class Server
{
    private :

        std::map<std::string, std::string> config;
        std::map<std::string, std::map<std::string, std::string>> locations;   
    
    public :

        Server();
        ~Server();
        Server(const Server & srcs);
        Server & operator=(const Server & rhs);
};

#endif