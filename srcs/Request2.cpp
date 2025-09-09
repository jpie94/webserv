/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/09 13:54:41 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Location.hpp"

void Request::parsRequest()
{
	std::string key, value, line, msg(this->_recieved);
	parsRequestLine(msg);
	if (this->_responseStatus == "200")
		resolvePath();
	this->printconfig();
	if (this->_responseStatus == "200")
		parsHeaders(msg);
	if (this->_responseStatus == "200")
		checkRequest();
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
	if (this->_protocol.compare("HTTP/1.1"))
		return (setStatus("505"));
	if (this->_path[0] != '/' || (this->_path[1] && this->_path[0] == '/' && this->_path[1] == '/'))
		return ((void)(std::cout << "400 Error -> 5\n"), setStatus("400"));
	if (this->_path.size() >= 4000)
		return (setStatus("414"));
	msg = msg.substr(line.size() + 1);
	if (!tmp.empty())
		return ((void)(std::cout << "400 Error -> 2\n"), setStatus("400"));
	// std::cout << "_methode= " << this->_methode << std::endl;
	// std::cout << "_path= " << this->_path << std::endl;
	// std::cout << "_protocol= " << this->_protocol << std::endl;
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
		if (count >= 4000)
			setStatus("431");
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
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
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
		if (this->_config.find("client_max_body_size") != this->_config.end() && this->_config["client_max_body_size"] != "0")
			if (this->_body_len > static_cast<size_t>(atoi(this->_config["client_max_body_size"].c_str())))
				return ((void)setStatus("413"));
		// std::cout << "msg.size()= " << msg.size() << std::endl;
		// std::cout << "body_len= " << this->_body_len << std::endl;
		if (msg[msg.size() - 1] == '\n')
			msg.erase(msg.size() - 1);
		if (msg[msg.size() - 1] == '\r')
			msg.erase(msg.size() - 1);
		// std::cout << "body2= " << msg << std::endl;
		this->_body = msg;
		// std::cout << "body at the end of parsBody()= " << this->_body << std::endl;
	}
}

int	Request::parsChunk(std::string &msg)
{
	std::string	chunk;
	size_t	pos, chunk_len;
	char* endpos;

	if (!msg.compare("0\r\n\r\n"))
	{
		msg = "";
		return (1);
	}
	chunk_len = std::strtol(msg.c_str(), &endpos, 16);
	pos = findCRLF(msg);
	if (pos == std::string::npos)
		return (setStatus("404"), 1);
	chunk = msg.substr(pos + 2, chunk_len);
	this->_body += chunk;
	msg = msg.substr(pos + chunk_len + 4);
	return (0);
}

void	Request::parsChunkedBody()
{
	std::string msg(this->_recieved);
	size_t pos = findCRLFCRLF(msg);

	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4 + this->_body.size());
	else
		msg.clear();
	std::string line, chunk;
	while (msg != "")
	{
		if (parsChunk(msg))
			break;
	}
}

int	Request::parsPart(std::string& msg, std::string& bound, std::string& endbound)
{
	size_t pos, endpos, count = 0;
	std::string part, line, key, value;
	std::map<std::string, std::string> partHeaders;
	pos = msg.find(bound);
	endpos = msg.find(endbound);
	if (endpos == std::string::npos || pos == endpos)	
		return (1);
	part = msg.substr(pos + bound.size());
	msg = msg.substr(part.size() + bound.size());
	pos = part.find(bound);
	part = part.substr(0, pos);
	std::cout << "part= " <<  part << std::endl;
	std::cout << "msg= " << msg << std::endl;
	std::istringstream ss(part);
	std::getline(ss, line, '\n');
	while (!line.empty() && line.find(':') != std::string::npos)
	{
		count += line.size() + 1;
		line = trim_white_spaces(line);
		if (line.empty())
			break;
		pos = line.find(':');
		if (!line.empty() && pos == std::string::npos)
			return (std::cout << "400 Error -> 9\n", setStatus("400"), 1);
		key = trim_white_spaces(line.substr(0, pos));
		strCapitalizer(key);
		value = trim_white_spaces(line.substr(pos + 1));
		if (this->_headers.find(key) != this->_headers.end())
			this->_headers[key] += " " + value;
		else
			this->_headers[key] = value;
		std::cout << "key= " << key << std::endl;
		std::cout << "value= " << value << std::endl;
		std::getline(ss, line, '\n');
	}
	part = part.substr(count);
	std::cout << "bodypart= " << part << std::endl;
	return (0);
	
}

void	Request::parsMultipart()
{
	std::string	bound, endbound, msg(this->_recieved);
	size_t pos = this->_headers["CONTENT-TYPE"].find("boundary=");

	bound = this->_headers["CONTENT-TYPE"].substr(pos + 9);
	removeQuotes(bound);
	bound = "--" + bound;
	pos = findCRLFCRLF(msg);
	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4 + this->_body.size());
	else
		msg.clear();
	while (pos!= std::string::npos)
		if (parsPart(msg, bound, endbound))
			return;
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
	else if(this->_config.find("allowed_methods") != this->_config.end())
		if(this->_config["allowed_methods"].find(this->_methode) == std::string::npos)
			return (setStatus("405"));
	if (this->_headers.find("HOST") == this->_headers.end())
		return ((void)(std::cout << "400 Error -> 6\n"), setStatus("400"));	
}

void Request::resolvePath()
{
	std::string finalPath, temPath = this->_path;
	std::vector<std::string> suffix;
	std::map<std::string, std::string>::iterator it = this->_config.find("root");
	std::map<std::string, Location*> temploc = _servers[this->_server_fd]->getLocations();
	if (it != this->_config.end())
		_ogRoot = it->second;
	while (!temPath.empty())
	{
		size_t i = temPath.rfind('/');
		suffix.push_back(temPath.substr(i));
	 	temPath = temPath.substr(0, i);
	}
	while (!suffix.empty())
	{
		temPath += *suffix.rbegin();
		finalPath += *suffix.rbegin();
		suffix.pop_back();
		std::map<std::string, Location*>::iterator MapLoc = temploc.find(temPath);
		if (MapLoc != temploc.end())
	 	{
			std::map<std::string, std::string> loc = MapLoc->second->getConfig();
	 		for (std::map<std::string, std::string>::iterator itLoc = loc.begin(); itLoc != loc.end(); itLoc++)
			{
				if(itLoc->first == "root")
				{
					if(itLoc->second != "/")
						finalPath = "/" + itLoc->second; 
					else
						finalPath.clear();
				}
	 			else
					this->_config[itLoc->first] = itLoc->second;
			}
			if (!MapLoc->second->getCgi().empty())
				this->_cgi = MapLoc->second->getCgi();
			if (!MapLoc->second->getErrPage().empty())
				this->_error_pages = MapLoc->second->getErrPage();
	 	}
	}
	finalPath = _ogRoot + finalPath;
	this->_path = finalPath;
}
// void	Request::addChunktoBody(std::string str)
// {
// 	std::ifstream::pos_type size;
// 	char * memblock;
// 	std::istringstream iss(str, std::ios::in|std::ios::binary|std::ios::ate);
	
// 	size = iss.tellg();
// 	memblock = new char [size];
// 	iss.seekg (0, std::ios::beg);
// 	iss.read (memblock, size);
// 	this->_body += iss.str();
// 	delete[] memblock;
// }

void 	Request::printURIConfig()
{
	std::cout << "REQUESTED URI CONFIG = " << this->_path << std::endl; 
	for (std::map<std::string, std::string>::iterator it = _config.begin(); it != _config.end(); it++)
		std::cout << it->first << " " << it->second << std::endl;
}