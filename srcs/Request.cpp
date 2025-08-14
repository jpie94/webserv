#include "Request.hpp"

/*****************	CANONICAL	*******************/

Request::Request() : _request_msg()
{
}

Request::Request(const Request& src)
{
	*this = src;
}

Request&	Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_methode = rhs._methode;
		this->_path = rhs._path;
		this->_protocol = rhs._protocol;
		this->_request_len = rhs._request_len;
	}
	return (*this);
}

Request::Request(std::string str) : _request_msg(str) {}

Request::~Request() {}


/*****************	MEMBER		*******************/

std::string	Request::trim_white_spaces(std::string str) const
{
	size_t	start = 0, end = 0;
	while (std::isspace(str[start]) && str[start])
		start++;
	end = start;
	while (!(std::isspace(str[end])) && str[end])
		end++;
	return (str.substr(start, end));
}

void	Request::parsRequestLine(std::string& msg)
{
	std::istringstream ss(msg);
	std::string line, tmp;

	std::getline(ss, line, '\n');
	this->_reqline_len = line.size() + 1;
	ss.str(line);
	ss >> this->_methode >> this->_path >> this->_protocol >> tmp;
	if (this->_methode.empty() || this->_path.empty() || this->_protocol.empty())
		Webserv::throw_error("Bad request : missing token in request line");
	msg = msg.substr(line.size() + 1);
	if (!tmp.empty())
		Webserv::throw_error("Bad request : invalid token on request line");
}

void	Request::parsHeaders(std::string& msg)
{
	std::istringstream	ss(msg);
	std::string		key, value, line;
	std::size_t		count = 0, found = std::string::npos;

	line = ss.str();
	if (line.find(CRLFCRLF) == std::string::npos)
		Webserv::throw_error("Bad request: no CRLF found");
	std::getline(ss, line, '\n');
	while (!line.empty())
	{
		count += line.size() + 1;
		if (line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		found = line.find(':');
		if (found == std::string::npos)
			Webserv::throw_error("Bad request : header without ':'");
		key = line.substr(0, found);
		value = trim_white_spaces(line.substr(found + 1));
		this->_headers[key] = value;
		// std::cout << "key = " << key << '\n';
		// std::cout << "value= " << value << '\n';
		std::getline(ss, line, '\n');
	}
	msg = ss.str();
	if (count + 1 < msg.size())
		msg = msg.substr(count + 1);
	this->_headers_len = count + 1;
}

void	Request::parsBody(std::string& msg)
{
	this->_body_len = msg.size();
	if (msg.size() - 1 != static_cast<unsigned int>(std::atoi(this->_headers["Content-Length"].c_str())))
		Webserv::throw_error("Bad Request: content lenght");
	if (msg[msg.size() - 1] == '\n')
		msg.erase(msg.size() - 1);
	if (msg[msg.size() - 1] == '\r')
		msg.erase(msg.size() - 1);
	this->_body = msg;
	// std::cout << "body= " << msg << std::endl;
}

void	Request::parsRequest()
{
	std::string	key, value, line, msg(this->_request_msg);

	this->_request_len = this->_request_msg.size();
	parsRequestLine(msg);
	parsHeaders(msg);
	if (this->_headers.find("Content-Length") != this->_headers.end())
		parsBody(msg);
	else if (msg.size() > 0)
		Webserv::throw_error("Bad request: invalid header");
	// std::cout << "\nrequest_len= " << this->_request_len << '\n' << "reqline= " << this->_reqline_len;
	// std::cout << ", headerslen= " << this->_headers_len << ", bodylen= " << this->_body_len << std::endl;
}
