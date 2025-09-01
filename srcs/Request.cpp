/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:19 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/01 12:39:23 by qsomarri         ###   ########.fr       */
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
	std::istringstream	ss(str);
	std::string	res, tmp;

	while (ss >> tmp)
		res += tmp + " ";
	return (res.substr(0, res.size() - 1));
}

static std::string	trim_CRLF(std::string str)
{
	size_t	end = str.size() - 1;

	while (end - 1 > 0 && str[end] == '\n' && str[end - 1] == '\r')
	{
			str = str.substr(0, str.size() - 2);
			end = str.size() -1;
	}
	return (str);
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
		return ((void)(std::cout << "400 Error -> 1\n"), setStatus("400"));
	msg = msg.substr(line.size() + 1);
	if (!tmp.empty())
		return ((void)(std::cout << "400 Error -> 2\n"), setStatus("400"));
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
		// std::cout << "1- line= " << line << std::endl;
		line = trim_white_spaces(line);
		if (line.empty())
			break;
		found = line.find(':');
		if (!line.empty() && found == std::string::npos)
			return ((void)(std::cout << "400 Error -> 3\n"), setStatus("400"));
		key = trim_white_spaces(line.substr(0, found));
		strCapitalizer(key);
		value = trim_white_spaces(line.substr(found + 1));
		if (this->_headers.find(key) != this->_headers.end())
			this->_headers[key] += " " + value;
		else
			this->_headers[key] = value;
		// std::cout << "key= " << key << std::endl;
		// std::cout << "value= " << value << std::endl;
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

	size_t pos = findCRLFCRLF(msg);

	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4);//wrong if body is large enought to be recieved in more than one recv
	else
		msg.clear();
	// std::cout << "body1= " << msg << std::endl;
	// std::cout << "status= " << this->_responseStatus << std::endl;
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
		// std::cout << "msg.size()= " << msg.size() << std::endl;
		// std::cout << "body_len= " << this->_body_len << std::endl;
		if (msg.size() != this->_body_len)//check if content-length is suppose to count CRLF
			return ((void)(std::cout << "400 Error -> 4\n"), setStatus("400"));
		if (msg[msg.size() - 1] == '\n')
			msg.erase(msg.size() - 1);
		if (msg[msg.size() - 1] == '\r')
			msg.erase(msg.size() - 1);
		// std::cout << "body2= " << msg << std::endl;
		this->_body = msg;
	}
}

void Request::checkRequest()
{
	if (this->_methode.compare("GET") && this->_methode.compare("POST") && this->_methode.compare("DELETE"))
	{
		if (this->_methode.compare("HEAD") && this->_methode.compare("PUT") && this->_methode.compare("CONNECT")
			&& this->_methode.compare("OPTIONS") && this->_methode.compare("TRACE") && this->_methode.compare("PATCH"))
			return (setStatus("405"));
		else
			return (setStatus("501"));
	}
	if (this->_path[0] != '/' || (this->_path[1] && this->_path[0] == '/' && this->_path[1] == '/'))
		return ((void)(std::cout << "400 Error -> 5\n"), setStatus("400"));
	if (this->_protocol.compare("HTTP/1.1"))
		return (setStatus("505"));
	if (this->_headers.find("HOST") == this->_headers.end())
		return ((void)(std::cout << "400 Error -> 6\n"), setStatus("400"));
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

void	Request::parsChunked()
{
	std::string msg(this->_recieved);
	size_t pos = findCRLFCRLF(msg);
	size_t		chunk_len;

	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4 + this->_body.size());
	else
		msg.clear();
	std::cout << "chunk= " << msg << std::endl;
	std::istringstream iss(msg);
	std::string token;
	iss >> token;
	std::cout << "tkn= " << token << std::endl;
	chunk_len = hexStringToInt(token);
	msg = msg.substr(token.size() + 2);
	std::cout << "msg without hex= " << msg << std::endl;
	trim_CRLF(msg);
	std::cout << "chunk_len= " << chunk_len << ", msg.size()= " << msg.size() << std::endl;
	if (msg.size() != chunk_len)
		return ((void)(std::cout << "400 Error -> 8\n"), setStatus("400"));
	addChunktoBody(msg);
}

void	Request::addChunktoBody(std::string str)
{
	std::ifstream::pos_type size;
	char * memblock;
	std::istringstream iss(str, std::ios::in|std::ios::binary|std::ios::ate);
	
	size = iss.tellg();
	memblock = new char [size];
	iss.seekg (0, std::ios::beg);
	iss.read (memblock, size);
	this->_body += memblock;
	delete[] memblock;
}
