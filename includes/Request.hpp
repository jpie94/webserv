/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/27 16:51:09 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>

#define CRLFCRLF "\n\n"
#define CRLF "\n"

class Request : public Client
{
	protected:
		std::map<std::string, std::string>	_headers;
		std::string				_body;
		std::string				_methode;
		std::string				_path;
		std::string				_protocol;
		std::string				_responseStatus;
	public:
		/* Canonical Form + Paramtric constructor */
							Request();
							Request(const Client&);
							Request(const Request&);
		Request&				operator=(const Request&);
		virtual					~Request();
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
		void					setStatus(std::string const& str);
};

#endif