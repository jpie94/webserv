#pragma once

#include "Webserv.hpp"
#include <cstdlib>
#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class Request : public Webserv
{
	private:
		const std::string			_request_msg;
		const std::string			_request_line;//delimited by whitespace
		std::map<std::string, std::string>	_headers;//name:value until empty line
		std::string				_body;//if there is a content-length header
		std::string				_methode;
		std::string				_path;
		std::string				_protocol;
		size_t					_request_len;
	public:
							Request(std::string str);
							~Request();
		std::string				getMethode() const;
		std::string				getPath() const;
		std::string				getProtocol() const;
		void					parsRequest();
		std::string				trim_white_spaces(std::string) const;
		void					parsRequestLine(std::string&);
		void					parsHeaders(std::string&);
		void					parsBody(std::string&);

};