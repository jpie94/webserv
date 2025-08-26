/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 17:22:01 by qsomarri         ###   ########.fr       */
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

class Answer;

class Request : public Client
{
	protected:
		const std::string					_request_msg;//can be a pb if there is a '\0' in the body-> try with char*
		std::map<std::string, std::string>	_headers;//name:value until empty line
		std::string				_body;//if there is a content-length header
		std::string				_methode;
		std::string				_path;
		std::string				_protocol;
		std::string				_responseStatus;
	public:
		/* Canonical Form + Paramtric constructor */
											Request();
											Request(std::string str);
											Request(const Request&);
		Request&							operator=(const Request&);
		virtual								~Request();
		/* Getters */
		std::string							getPath() const;
		std::string							getProtocol() const;
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