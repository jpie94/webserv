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

static std::string	trim_white_spaces(std::string str)
{
	size_t	start = 0, end = 0;
	while (std::isspace(str[start]) && str[start])
		start++;
	end = str.size() - 1;
	while (std::isspace(str[end]) && end > 0)
		end--;
	if (end < str.size())
		end++;
	if (start <= end)
		return (str.substr(start, end));
	return ("");
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
	std::size_t		count = 0, count_double = 0, found = std::string::npos;

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
		key = trim_white_spaces(line.substr(0, found));
		value = trim_white_spaces(line.substr(found + 1));
		if (this->_headers.find(key) != this->_headers.end())
		{
			this->_headers[key] += " " + value;
			count_double += found;
		}
		else
			this->_headers[key] = value;
		// std::cout << "key = " << key << '\n';
		// std::cout << "value= " << value << '\n';
		std::getline(ss, line, '\n');
	}
	// for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	// 	std::cout << "[" << it->first << "]-> " << it->second << '\n';
	msg = ss.str();
	if (count + 1 < msg.size())
		msg = msg.substr(count + 1);
	this->_headers_len = count + 1 - count_double;
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

void	Request::checkRequest()
{
	if (this->_methode.compare("GET") && this->_methode.compare("POST") && this->_methode.compare("DELETE"))
	{
		if (this->_methode.compare("HEAD") && this->_methode.compare("PUT") && this->_methode.compare("CONNECT")
			&& this->_methode.compare("OPTIONS") && this->_methode.compare("TRACE") && this->_methode.compare("PATCH"))
			Webserv::throw_error("Error: Invalide request Methode");
		else
			Webserv::throw_error("Error: Webserv doesn't handle this methode");
	}
	// if (this->_path[0] == '/')
	// 	this->_path = this->_path.substr(1);
	DIR* dir = opendir(this->_path.c_str());//certainement qu'il faut quand meme essaye de faire la requete
	if (!dir)
		throw_error("Error: invalid request Path");
	if (closedir(dir) < 0)
		throw_error("Error: closedir");
	if (this->_protocol.compare("HTTP/1.1") && this->_protocol.compare("HTTP/0.9") && this->_protocol.compare("HTTP/1.0"))
		Webserv::throw_error("Error: Wrong HTTP request Protocol");
	if (this->_headers.find("Host") == this->_headers.end())
		Webserv::throw_error("Error: Bad HTTP request - missing \'Host\' header");
}

void	Request::parsRequest()
{
	std::string	key, value, line, msg(this->_request_msg);

	this->_request_len = this->_request_msg.size();
	parsRequestLine(msg);
	parsHeaders(msg);
	checkRequest();
	if (this->_headers.find("Content-Length") != this->_headers.end())
		parsBody(msg);
	else if (msg.size() > 0)
		Webserv::throw_error("Bad request: invalid header");
	// std::cout << "\nrequest_len= " << this->_request_len << '\n' << "reqline= " << this->_reqline_len;
	// std::cout << ", headerslen= " << this->_headers_len << ", bodylen= " << this->_body_len << std::endl;
}
