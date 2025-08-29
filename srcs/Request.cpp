/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:19 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/29 14:23:19 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Response.hpp"
#include "Client.hpp"

/*****************	CANONICAL	*******************/

Request::Request() : Client(), _headers_len(), _request_line_len(), _body_len(), _body(), _methode(), _path(), _protocol(), _responseStatus("200") {}

Request::Request(const Request &src)
{
	*this = src;
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_methode = rhs._methode;
		this->_path = rhs._path;
		this->_protocol = rhs._protocol;
		this->_responseStatus = rhs._responseStatus;
		this->_body_len = rhs._body_len;
		this->_headers_len = rhs._headers_len;
		this->_request_line_len = rhs._request_line_len;
	}
	return (*this);
}

Request::Request(const Client &client) : Client(client), _headers_len(), _request_line_len(), _body_len(), _body(), _methode(), _path(), _protocol(), _responseStatus("200") {}

Request::~Request() {}

/*****************	CLASS UTILS	*******************/

static void strCapitalizer(std::string &str)
{
	size_t i = -1;
	while (str[++i])
		str[i] = static_cast<char>(std::toupper(str[i]));
}

static std::string trim_white_spaces(std::string str)
{
	size_t start = 0, end = 0;
	while (std::isspace(str[start]) && str[start])
		start++;
	end = str.size() - 1;
	while (std::isspace(str[end]) && end > 0)
		end--;
	if (end < str.size())
		end++;
	if (start <= end)
		return (str.substr(start, end - start));
	return ("");
}

/*****************	MEMBER		*******************/

std::string Request::getProtocol() const
{
	return (this->_protocol);
}

void Request::setRecieved(std::string &str)
{
	this->_recieved = str;
}

std::string Request::getBody() const
{
	return (this->_body);
}

std::map<std::string, std::string> Request::getHeaders() const
{
	return (this->_headers);
}

size_t Request::getBodyLen() const
{
	return (this->_body_len);
}

size_t Request::getHeadersLen() const
{
	return (this->_headers_len);
}

size_t Request::getRequestLineLen() const
{
	return (this->_request_line_len);
}

void Request::parsRequestLine(std::string &msg)
{
	std::istringstream ss(msg);
	std::string line, tmp;

	std::getline(ss, line, '\n');
	this->_request_line_len += line.size() + 1;
	ss.str(line);
	ss >> this->_methode >> this->_path >> this->_protocol >> tmp;
	if (this->_methode.empty() || this->_path.empty() || this->_protocol.empty())
		return ((void)(std::cout << "1" << std::endl), setStatus("400"));
	msg = msg.substr(line.size() + 1);
	if (!tmp.empty())
		return ((void)(std::cout << "2" << std::endl), setStatus("400"));
}

void Request::parsHeaders(std::string &msg)
{
	std::istringstream ss(msg);
	std::string key, value, line;
	std::size_t count = 0, found = std::string::npos;

	line = ss.str();
	std::getline(ss, line, '\n');
	while (!line.empty())
	{
		count += line.size() + 1;
		// if (line[line.size() - 1] == '\r')
		// 	line.erase(line.size() - 1);
		line = trim_white_spaces(line);
		std::cout << "line= " << line << std::endl;
		found = line.find(':');
		if (!line.empty() && found == std::string::npos)
			return ((void)(std::cout << "3" << std::endl), setStatus("400"));
		key = trim_white_spaces(line.substr(0, found));
		strCapitalizer(key);
		value = trim_white_spaces(line.substr(found + 1));
		if (this->_headers.find(key) != this->_headers.end())
			this->_headers[key] += " " + value;
		else
			this->_headers[key] = value;
		std::getline(ss, line, '\n');
	}
	msg = ss.str();
	if (count + 1 <= msg.size())
		msg = msg.substr(count + 1);
	this->_headers_len += count;
}

void Request::parsBody()
{
	std::string msg(this->_recieved);

	size_t pos = msg.find(CRLFCRLF);

	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4);
	else
		msg.clear();
	if (this->_responseStatus == "200" && this->_headers.find("Content-Length") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["Content-Length"].c_str());
		if (msg.size() - 1 != this->_body_len)
			return ((void)(std::cout << "4" << std::endl), setStatus("400"));
		if (msg[msg.size() - 1] == '\n')
			msg.erase(msg.size() - 1);
		if (msg[msg.size() - 1] == '\r')
			msg.erase(msg.size() - 1);
		this->_body = msg;
	}
	else if (this->_responseStatus == "200" && msg.size() > 0)
		return ((void)(std::cout << "5" << std::endl), setStatus("400"));
}

void Request::checkRequest()
{
	if (this->_methode.compare("GET") && this->_methode.compare("POST") && this->_methode.compare("DELETE"))
	{
		if (this->_methode.compare("HEAD") && this->_methode.compare("PUT") && this->_methode.compare("CONNECT") && this->_methode.compare("OPTIONS") && this->_methode.compare("TRACE") && this->_methode.compare("PATCH"))
			return (setStatus("501"));
		else
			return (setStatus("405"));
	}
	if (this->_path[0] != '/' || (this->_path[1] && this->_path[0] == '/' && this->_path[1] == '/'))
		return ((void)(std::cout << "6" << std::endl), setStatus("400"));
	if (this->_protocol.compare("HTTP/1.1"))
		return (setStatus("505"));
	if (this->_headers.find("HOST") == this->_headers.end())
		return ((void)(std::cout << "7" << std::endl), setStatus("400"));
}

void Request::parsRequest()
{
	std::string key, value, line, msg(this->_recieved);
	parsRequestLine(msg);
	if (this->_responseStatus == "200")
		parsHeaders(msg);
	if (this->_responseStatus == "200")
		checkRequest();
}
void Request::setStatus(std::string const &str)
{
	this->_responseStatus = str;
}

std::string	Request::getRecieved() const
{
	return (this->_recieved);
}