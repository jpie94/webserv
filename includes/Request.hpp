/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:58 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/30 19:15:43 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include <cstdlib>
#include <dirent.h>
#include <stdio.h>

#define CRLFCRLF "\r\n\r\n"
#define CRLF "\r\n"

class Request : public Client
{
	protected:
		size_t								_headers_len;
		size_t								_request_line_len;
		size_t								_body_len;
		size_t								_msg_size;
		size_t								_part_size;
		std::map<std::string, std::string>	_headers;
		static std::map<std::string, std::string>	_files;//name->tmp_path pour gere les fichiers temporaires
		std::string							_body;
		std::string							_methode;
		std::string							_path;
		std::string							_protocol;
		std::string							_responseStatus;
		std::string							_ogRoot;
		bool								_isCGI;
		std::string							_CGI_bin_path;
		std::string							_CGI_script;
		std::string							_CGI_pathInfo;
		std::string							_CGI_querry;
		std::string							_CGIinterpret;

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
		bool								get_isCGI() const;
		std::string							getStatus() const;
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
		void								setRecieved(std::string& str);
		void								addChunktoBody(std::string str);
		void 								printURIConfig();
		void								parsMultipart();
		int									parsPart(char* &, std::string&, std::string&);
		void								check_cgi();
		void								getCgiScript();
		void								checkCGIExt();
		void								set_isCGIFalse();
		int									extractPart(char* &msg, const std::string &bound, const std::string &endbound, char* &part, size_t &sep_pos);
		std::map<std::string, std::string>	makeHeadersMap(char* &part, size_t& sep_pos);
		int									handleContent(std::map<std::string, std::string>& headers_map, std::string& body_part);
		void 								clearTmpFiles();
};

#endif