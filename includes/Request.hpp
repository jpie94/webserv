/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/25 18:16:15 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

#define CRLFCRLF "\n\n"
#define CRLF "\n"

class Answer;

class Request : public Client
{
	protected:
		const std::string			_request_msg;//can be a pb if there is a '\0' in the body-> try with char*
		std::map<std::string, std::string>	_headers;//name:value until empty line
		std::string				_body;//if there is a content-length header
		std::string				_methode;
		std::string				_path;
		std::string				_protocol;
	public:
		/* Canonical Form + Paramtric constructor */
							~Request();
							Request();
							Request(const Request&);
		Request&				operator=(const Request&);
							Request(std::string str);
		/* Getters */
		std::string				getPath() const;
		std::string				getProtocol() const;
		/* Member Functions */
		void					getMethode() const;
		void					postMethode() const;
		void					deleteMethode() const;
		void					parsRequest();
		void					parsRequestLine(std::string&);
		void					parsHeaders(std::string&);
		void					parsBody(std::string&);
		void					checkRequest();
		void					makeResponse();
};