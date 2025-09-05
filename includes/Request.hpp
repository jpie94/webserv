/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/05 15:56:51 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include <cstdlib>
#include <dirent.h>

#define CRLFCRLF "\r\n\r\n"
#define CRLF "\r\n"

class Request : public Client
{
	protected:
		size_t								_headers_len;
		size_t								_request_line_len;
		size_t								_body_len;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_methode;
		std::string							_path;
		std::string							_protocol;
		std::string							_responseStatus;
		std::string							_ogRoot;

	public:
		/* Canonical Form + Paramtric constructor */
											Request();
											Request(const Client &);
											Request(const Request &);
		Request								&operator=(const Request &);
		virtual								~Request();
		/* Getters */
		std::string							getPath() const;
		std::string							getProtocol() const;
		std::string							getBody() const;
		std::map<std::string, std::string>	getHeaders() const;
		size_t								getBodyLen() const;
		size_t								getHeadersLen() const;
		size_t								getRequestLineLen() const;
		std::string							getRecieved() const;
		/* Member Functions */
		void								parsRequest();
		void								parsRequestLine(std::string &);
		void								resolvePath();
		void								parsHeaders(std::string &);
		void								parsBody();
		void								parsChunkedBody();
		int									parsChunk(std::string&);
		void								checkRequest();
		void								setStatus(std::string const &str);
		void								setRecieved(std::string &);
		void								addChunktoBody(std::string str);
		void 								printURIConfig();
};

#endif