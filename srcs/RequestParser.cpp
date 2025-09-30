/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/30 19:10:16 by qsomarri         ###   ########.fr       */
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
	if (count + 1 <= this->_msg_size)
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
		if (!msg.empty() && msg[this->_msg_size - 1] == '\n') // Je comprends pas pk le /R/N est supprime ici
			msg.erase(this->_msg_size - 1);
		if (msg[!msg.empty() && this->_msg_size - 1] == '\r')// C est chelou le corchet la
			msg.erase(this->_msg_size - 1);
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

int	Request::extractPart(char* &msg, const std::string &bound, const std::string &endbound, char* &part, size_t &sep_pos)
{
	size_t start = find_mem(msg, bound, this->_count);
	size_t end = find_mem(msg, endbound, this->_count - start);
	if (start == end)// end == std::string::npos ||
		return (std::cout << "start= " << start << ", end= " << end << std::endl, 1);
	part = msg.substr(start + bound.size());
	std::cout << "start= " << start << std::endl;
	std::cout << "this->_part_size= " << this->_part_size << std::endl;
	if (this->_part_size >= 2 && part[0] == '-' && part[1] == '-')
		return (std::cout << "error 1" << std::endl, 1);
	if (this->_part_size >= 2 && part[0] == '\r' && part[1] == '\n')
		part = part.substr(2);
	sep_pos = find_mem(part, bound, sizeof(part));
	if (sep_pos == std::string::npos)
		return (std::cout << "error 2" << std::endl, setStatus("400"), 1);
	part = part.substr(0, sep_pos);
	msg = (this->_part_size + bound.size() < this->_msg_size ? msg.substr(sep_pos + bound.size()) : std::string());
	return (0);
}

std::map<std::string, std::string>	Request::makeHeadersMap(char* &part, size_t& sep_pos)
{
	std::map<std::string, std::string> headers_map;
	std::string header_line;

	sep_pos = find_mem(part, CRLFCRLF, this->_part_size);
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

int Request::parsPart(char* &msg, std::string& bound, std::string& endbound)
{
	char* part = new char[this->_count];
	part = this->_rcv_bin;
	size_t	sep_pos;

	if (extractPart(msg, bound, endbound, part, sep_pos))
		return (std::cout << "error 0" << std::endl, 1);
	//std::cout << "part= " << part << std::endl;
	//std::cout << "sep_pos= " << sep_pos << std::endl;
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
	this->_msg_size = this->_count;
	char*	msg = new char [this->_count];
	msg = this->_rcv_bin;
	std::string	bound, endbound;
	size_t pos = this->_headers["CONTENT-TYPE"].find("boundary=");

	bound = this->_headers["CONTENT-TYPE"].substr(pos + 9);
	removeQuotes(bound);
	bound = "--" + bound;
	endbound = bound + "--";
	pos = findCRLFCRLF(msg);
	std::cout << "recieved size= " << this->_count << std::endl;
	std::cout << "msg size 1= " << this->_msg_size << std::endl;
	if (pos != std::string::npos)
		msg = msg.substr(find_mem(msg, CRLFCRLF, this->_msg_size) + 4 + this->_body.size());
	else
		delete [] msg;
	//std::cout << "msg1= " << msg << std::endl;
	std::cout << "msg size 2= " << this->_msg_size << std::endl;
	while (pos!= std::string::npos)
	{
		if (parsPart(msg, bound, endbound))
		{
			std::cerr << "parsPart failed !" << std::endl;
			return;
		}
	}
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

