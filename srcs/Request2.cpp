/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request2.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/29 19:28:23 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Location.hpp"

void Request::parsRequest()
{
	std::string key, value, line, msg(this->_recieved);
	parsRequestLine(msg);
	resolvePath();
	this->printconfig();
	parsHeaders(msg);
	checkRequest();
	check_cgi();
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
		std::getline(ss, line, '\n');
	}
	msg = ss.str();
	if (count + 1 <= msg.size())
		msg = msg.substr(count + 1);
	this->_headers_len += count;
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
		std::cout << "content-l= " << this->_body_len << std::endl;
	}
}

void Request::parsBody()
{
	std::string msg(this->_recieved);

	size_t pos = findCRLFCRLF(msg);

	if (pos != std::string::npos) // Du coup on a deja pos, on pourrait eviter de refaire un find et juste ecrire pos + 4
		msg = msg.substr(msg.find(CRLFCRLF) + 4);//wrong if body is large enought to be recieved in more than one recv
	else
		msg.clear();	
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
		if (this->_config.find("client_max_body_size") != this->_config.end() && this->_config["client_max_body_size"] != "0")
			if (this->_body_len > static_cast<size_t>(atoi(this->_config["client_max_body_size"].c_str())))
				return ((void)setStatus("413"));
		if (!msg.empty() && msg[msg.size() - 1] == '\n') // Je comprends pas pk le /R/N est supprime ici
			msg.erase(msg.size() - 1);
		if (msg[!msg.empty() && msg.size() - 1] == '\r')// C est chelou le corchet la
			msg.erase(msg.size() - 1);
		this->_body = msg;
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

// int	Request::parsPart(std::string& msg, std::string& bound, std::string& endbound)
// {
// 	size_t pos, endpos, count = 0;
// 	std::string part, line, key, value;
// 	std::map<std::string, std::string> partHeaders;
// 	pos = msg.find(bound);
// 	endpos = msg.find(endbound);
// 	if (endpos == std::string::npos || pos == endpos)	
// 		return (1);
// 	part = msg.substr(pos + bound.size());
// 	pos = part.find(bound);
// 	part = part.substr(2, pos - 2);
// 	msg = msg.substr(part.size() + bound.size());
// 	// std::cout << "part= " <<  part << std::endl;
// 	// std::cout << "msg= " << msg << std::endl;
// 	std::istringstream ss(part);
// 	std::getline(ss, line, '\n');
// 	while (!line.empty() && line.find(':') != std::string::npos)
// 	{
// 		count += line.size() + 1;
// 		line = trim_white_spaces(line);
// 		if (line.empty())
// 			break;
// 		pos = line.find(':');
// 		if (!line.empty() && pos == std::string::npos)
// 			return (std::cout << "400 Error -> 9\n", setStatus("400"), 1);
// 		key = trim_white_spaces(line.substr(0, pos));
// 		strCapitalizer(key);
// 		value = trim_white_spaces(line.substr(pos + 1));
// 		if (this->_headers.find(key) != this->_headers.end())
// 			this->_headers[key] += " " + value;
// 		else
// 			this->_headers[key] = value;//try to get all the name= in Content-disposition
// 		// std::cout << "key= " << key << std::endl;
// 		// std::cout << "value= " << value << std::endl;
// 		std::cout << "name= " << getName(value, "name=") << std::endl;//search all the "name=" in Content-disposition
// 		std::getline(ss, line, '\n');
// 	}
// 	part = part.substr(count);
// 	part = trim_white_spaces(part);
// 	// std::cout << "bodypart= " << part << std::endl;
// 	return (0);
// }

size_t find_substring(const std::string &str, const std::string &substr)
{
	if (substr.empty() || str.size() < substr.size())
		return (std::string::npos);
	for (size_t i = 0; i <= str.size() - substr.size(); ++i)
		if (std::memcmp(&str[i], &substr[0], substr.size()) == 0)
			return (i);
	return (std::string::npos);
}

int	Request::extractPart(std::string &msg, const std::string &bound, const std::string &endbound, std::string &part, size_t &sep_pos)
{
	size_t start = find_substring(msg, bound);
	size_t end = find_substring(msg, endbound);
	if (start == end)// end == std::string::npos ||
		return (std::cout << "start= " << start << ", end= " << end << std::endl, 1);
	part = msg.substr(start + bound.size());
	std::cout << "start= " << start << std::endl;
	std::cout << "part.size()= " << part.size() << std::endl;
	if (part.size() >= 2 && part[0] == '-' && part[1] == '-')
		return (std::cout << "error 1" << std::endl, 1);
	if (part.size() >= 2 && part[0] == '\r' && part[1] == '\n')
		part = part.substr(2);
	sep_pos = find_substring(part, bound);
	if (sep_pos == std::string::npos)
		return (std::cout << "error 2" << std::endl, setStatus("400"), 1);
	part = part.substr(0, sep_pos);
	msg = (part.size() + bound.size() < msg.size() ? msg.substr(sep_pos + bound.size()) : std::string());
	return (0);
}

// int	Request::extractPart(std::string& msg, std::string& bound, std::string& endbound, std::string& part, size_t& sep_pos)
// {
// 	size_t start = msg.find(bound);
// 	size_t end = msg.find(endbound);

// 	if (end == std::string::npos || start == end)
//         	return (/*std::cout << "start= " << start << ", end= " << end << std::endl, */1);
// 	part = msg.substr(start + bound.size());
// 	if (part.substr(0, 2) == "--")
// 		return (std::cout << "ok 2" << std::endl, 1);
// 	if (part.substr(0, 2) == CRLF)
// 		part = part.substr(2);
// 	//std::cout << "part00= " << part << std::endl;
// 	sep_pos = part.find(bound);
// 	if (sep_pos == std::string::npos)
// 		return (std::cout << "400 Error -> 9\n", setStatus("400"), 1);
// 	//std::cout << "sep_pos00= " << sep_pos << std::endl;
// 	part = part.substr(0, sep_pos);
// 	msg = msg.substr(sep_pos + bound.size());
// 	//std::cout << "msg2= " << msg << std::endl;
// 	return (0);
// }

std::map<std::string, std::string>	Request::makeHeadersMap(std::string& part, size_t& sep_pos)
{
	std::map<std::string, std::string> headers_map;
	std::string header_line;

	sep_pos = part.find(CRLFCRLF);
	std::string headers_block = part.substr(0, sep_pos);
	std::istringstream headers_ss(headers_block);
	//std::cout << "hblock= " << headers_block << std::endl;
	while (std::getline(headers_ss, header_line))
	{
		//std::cout << "line= " << header_line << std::endl;
		header_line = trim_white_spaces(header_line);
		if (header_line.empty())
			break;
		size_t colon = header_line.find(':');
		if (colon == std::string::npos)
			return (std::cout << "400 Error -> 10\n", setStatus("400"), std::map<std::string, std::string>());
		std::string key = trim_white_spaces(header_line.substr(0, colon));
		strCapitalizer(key);
		std::string value = trim_white_spaces(header_line.substr(colon + 1));
		headers_map[key] = value;
	}
	return (headers_map);
}

int	Request::handleContent(std::map<std::string, std::string>& headers_map, std::string& body_part)
{
	std::string name = getName(headers_map["CONTENT-DISPOSITION"], "name=");
	std::string filename = getName(headers_map["CONTENT-DISPOSITION"], "filename=");
	if (name.empty())
		return (std::cout << "400 Error -> 11\n", setStatus("400"), 1);
	if (!filename.empty())
	{
		std::string tmp_path = "/tmp/upload_tempfile_" + generateRandomName();
		std::ofstream file(tmp_path.c_str(), std::ios::binary);
		if (!file.is_open())
			return (std::cout << "error 3" << std::endl, setStatus("500"), 1);
		file << body_part;
		file.close();
		this->_files[name] = tmp_path;
	}
	else
	{
		std::ofstream csv("/tmp/form_data.csv", std::ios::app);
		if (!csv.is_open())
			return (setStatus("500"), 1);
		body_part = trim_white_spaces(body_part);
		if (body_part.find(',') != std::string::npos)
			body_part = "\"" + body_part + "\"";
		csv << name << "," << body_part << "\n";
		csv.close();
	}
	return (0);
}

int Request::parsPart(std::string& msg, std::string& bound, std::string& endbound)
{
	std::string part;
	size_t	sep_pos;

	if (extractPart(msg, bound, endbound, part, sep_pos))
		return (std::cout << "error 0" << std::endl, 1);
	std::cout << "part= " << part << std::endl;
	 std::cout << "sep_pos= " << sep_pos << std::endl;
	std::map<std::string, std::string> headers_map = makeHeadersMap(part, sep_pos);
	for (std::map<std::string, std::string>::iterator it = headers_map.begin(); it != headers_map.end(); ++it)
		std::cout << it->first << " : " << it->second << std::endl;
	if (headers_map.find("CONTENT-DISPOSITION") == headers_map.end())
		return (std::cout << "400 Error -> 12\n", setStatus("400"), 1);
	std::string body_part = part.substr(sep_pos + 4);
	if (handleContent(headers_map, body_part))
		return (std::cout << "error 4" << std::endl, 1);
	return (0);
}

void	Request::parsMultipart()
{
	std::string	bound, endbound, msg(this->_recieved);
	size_t pos = this->_headers["CONTENT-TYPE"].find("boundary=");

	bound = this->_headers["CONTENT-TYPE"].substr(pos + 9);
	removeQuotes(bound);
	bound = "--" + bound;
	endbound = bound + "--";
	pos = findCRLFCRLF(msg);
	if (pos != std::string::npos)
		msg = msg.substr(msg.find(CRLFCRLF) + 4 + this->_body.size());
	else
		msg.clear();
	std::cout << "msg1= " << msg << std::endl;
	while (pos!= std::string::npos)
		if (parsPart(msg, bound, endbound))
		{
			std::cerr << "parsPart failed !" << std::endl;
			return;
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
				this->_cgis = MapLoc->second->getCgi();
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

void	Request::check_cgi()
{
	std::string temp, extension;
	temp = this->_path;
	size_t pos = temp.find(_ogRoot);
	if (pos != std::string::npos)
		temp = temp.substr(_ogRoot.size() + 1);
	pos = temp.find("/");
	if (pos != std::string::npos)
		temp = temp.substr(0, pos);
	if (temp == "cgi-bin")
	{
		this->_CGI_bin_path = _ogRoot + "/" + temp + "/";
		this->getCgiScript();
		this->checkCGIExt();
	}
	else 
		this->_isCGI = false;
}

void 	Request::getCgiScript()
{
	std::string temp = this->_path.substr(_CGI_bin_path.size());
	size_t pos = temp.find("?");
	if (pos != std::string::npos)
	{
		this->_CGI_querry= temp.substr(pos + 1);
		temp = temp.substr(0, pos);
		pos = temp.find("/");
		if (pos != std::string::npos)
		{
			this->_CGI_pathInfo = temp.substr(pos + 1);
			temp = temp.substr(0, pos);
		}
	}
	else 
	{
		pos = temp.find("/");
		if (pos != std::string::npos)
		{
			this->_CGI_pathInfo = temp.substr(pos + 1);
			temp = temp.substr(0, pos);	
		}
	}
	this->_CGI_script = temp;
}

void	Request::checkCGIExt()
{
	size_t pos = this->_CGI_script.rfind(".");
	if (pos != std::string::npos)
	{
		std::string extension = this->_CGI_script.substr(pos, this->_CGI_script.size() - pos);
		std::map<std::string, std::string>::iterator it = this->_cgis.find(extension);
		if (it != this->_cgis.end())
		{
			this->_CGIinterpret = it->second;
			this->_isCGI = true;
			return ;
		}
	}
	return (this->_isCGI=false, setStatus("500"));
}

void Request::clearTmpFiles()
{
	for (std::map<std::string, std::string>::iterator it = _files.begin(); it != _files.end(); ++it)
		std::remove(it->second.c_str());
	_files.clear();
}