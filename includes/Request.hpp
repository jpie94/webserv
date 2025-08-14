#pragma once

#include "Webserv.hpp"
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#define CRLFCRLF "\n\n"

class Request : public Webserv
{
	private:
		const std::string			_request_msg;
		std::map<std::string, std::string>	_headers;//name:value until empty line
		std::string				_body;//if there is a content-length header
		std::string				_methode;
		std::string				_path;
		std::string				_protocol;
		size_t					_request_len;
		size_t					_reqline_len;
		size_t					_headers_len;
		size_t					_body_len;
	public:
		/* Canonical Form + Paramtric constructor */
							~Request();
							Request();
							Request(const Request&);
		Request&				operator=(const Request&);
							Request(std::string str);
		/* Getters */
		std::string				getMethode() const;
		std::string				getPath() const;
		std::string				getProtocol() const;
		/* Member Functions */
		void					parsRequest();
		void					parsRequestLine(std::string&);
		void					parsHeaders(std::string&);
		void					parsBody(std::string&);
		std::string				trim_white_spaces(std::string) const;

};