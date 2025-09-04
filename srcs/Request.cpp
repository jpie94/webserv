/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:19 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/04 18:05:15 by qsomarri         ###   ########.fr       */
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
		this->_config = rhs._config;
		this->_locations = rhs._locations;
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



/*****************	GETTERS / SETTERS		*******************/

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

std::string	Request::getPath() const
{
	return (this->_path);
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

void Request::setStatus(std::string const &str)
{
	this->_responseStatus = str;
}

std::string	Request::getRecieved() const
{
	return (this->_recieved);
}
