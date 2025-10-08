/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:01:59 by qsomarri          #+#    #+#             */
/*   Updated: 2025/10/08 19:22:48 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

void Request::parsRequest()
{
	std::string key, value, line, msg(this->_recieved);
	parsRequestLine(msg);
	resolvePath();
	//this->printconfig();
	parsHeaders(msg);
	checkRequest();
	check_cgi();
}

void Request::parsRequestLine(std::string &msg)
{
	std::istringstream	ss(msg);
	std::string			line, tmp;

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
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
	if (this->_config.find("client_max_body_size") != this->_config.end() && this->_config["client_max_body_size"] != "0")
		if (this->_body_len > static_cast<size_t>(atoi(this->_config["client_max_body_size"].c_str())))
			return (setStatus("413"));
	parsCookie();
}

void Request::parsBody()
{
	size_t pos = find_mem(this->_rcv_binary, CRLFCRLF);
	if (pos == std::string::npos)
		return;
	this->_rcv_binary.erase(this->_rcv_binary.begin(), this->_rcv_binary.begin() + pos + 4);
	if (this->_responseStatus == "200" && this->_headers.find("CONTENT-LENGTH") != this->_headers.end())
	{
		this->_body_len = std::atoi(this->_headers["CONTENT-LENGTH"].c_str());
		if (this->_rcv_binary.back() == '\n')
			this->_rcv_binary.pop_back();
		if (this->_rcv_binary.back() == '\r')
			this->_rcv_binary.pop_back();
		this->_body = this->_rcv_binary;
		printVect(this->_body);
	}
}

int	Request::parsChunk(std::vector<char>& msg)
{
	size_t	pos, chunk_len;
	char* endpos;

	if (!find_mem(msg, "0\r\n\r\n"))
	{
		msg.clear();
		return (1);
	}
	chunk_len = std::strtol(msg.data(), &endpos, 16);
	pos = find_mem(msg, CRLF);
	if (pos == std::string::npos)
		return (std::cout << "400 Error 123\n", setStatus("400"), 1);
	std::vector<char> chunk(msg.begin() + pos + 2, msg.begin() + pos + 2 + chunk_len);
	this->_body.insert(this->_body.end(), chunk.begin(), chunk.end());
	if (msg.size() >= pos + chunk_len + 4)
		msg.erase(msg.begin(), msg.begin() + pos + chunk_len + 4);
	else
		return (std::cout << "400 Error 456\n", setStatus("400"), 1);
	return (0);
}

void	Request::parsChunkedBody()
{
	size_t pos = find_mem(this->_rcv_binary, CRLFCRLF);
	if (pos == std::string::npos)
		return;
	this->_rcv_binary.erase(this->_rcv_binary.begin(), this->_rcv_binary.begin() + pos + 4);
	while (this->_rcv_binary.size())
	{
		if (parsChunk(this->_rcv_binary))
			break;
	}
}

int	Request::extractPart(std::vector<char>& msg, const std::string &bound, std::vector<char>& part, size_t &sep_pos)
{
	size_t pos = find_mem(msg, bound);
	if (pos == std::string::npos)
		return(std::cout << "400 error 3000" << std::endl, setStatus("400"), 1);
	part = std::vector<char>(msg.begin() + pos + bound.size(), msg.end());
	if (part.size() >= 2 && part[0] == '-' && part[1] == '-')
		return (1);
	if (part.size() >= 2 && part.data()[0] == '\r' && part.data()[1] == '\n')
	{
		pos = find_mem(part, CRLF);
		if (pos == std::string::npos)
			return(std::cout << "400 error 4000" << std::endl, setStatus("400"), 1);
		part.erase(part.begin(), part.begin() + pos + 2);
	}
	sep_pos = find_mem(part, bound);
	if (sep_pos == std::string::npos)
		return (std::cout << "400 error 2000" << std::endl, setStatus("400"), 1);
	part.erase(part.begin() + sep_pos, part.end());
	if (part.size() + bound.size() < msg.size())
		msg.erase(msg.begin(), msg.begin() + part.size());
	else
		msg.clear();
	return (0);
}

std::map<std::string, std::string>	Request::makeHeadersMap(std::vector<char> part, size_t& sep_pos)
{
	std::map<std::string, std::string> headers_map;
	std::string header_line;

	sep_pos = find_mem(part, CRLFCRLF);
	if (sep_pos == std::string::npos)
		return (std::cout << "400 error 5000" << std::endl, setStatus("400"), std::map<std::string, std::string>());
	part.erase(part.begin() + sep_pos, part.end());;
	part.push_back('\0');
	std::istringstream headers_ss(part.data());
	while (std::getline(headers_ss, header_line))
	{
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

int	Request::handleContent(std::map<std::string, std::string>& headers_map, std::vector<char>& body_part)
{
	std::string name = getName(headers_map["CONTENT-DISPOSITION"], "name=");
	std::string filename = getName(headers_map["CONTENT-DISPOSITION"], "filename=");
	if (name.empty())
		return (std::cout << "400 Error -> 11\n", setStatus("400"), 1);
	if (!filename.empty())
	{
		std::string tmp_path = "/tmp/upload_tempfile_" + generateRandomName(10);
		std::ofstream file(tmp_path.c_str(), std::ios::binary);
		if (!file.is_open() || file.fail())
			return (std::cout << "error 3" << std::endl, setStatus("500"), 1);
		file.write(body_part.data(), body_part.size());
		file.close();
		this->_files[name] = tmp_path;
	}
	else
	{
		body_part.push_back('\0');
		std::string body_str(body_part.data());
		std::string tmp_path = _ogRoot + "/tmp/form_data.csv";
		std::ofstream csv(tmp_path.c_str(), std::ios::app);
		if (!csv.is_open())
			return (std::cout << "Error 500 in handleContent\n", setStatus("500"), 1);
		body_str = trim_white_spaces(body_str);
		if (body_str.find(',') != std::string::npos)
			body_str = "\"" + body_str + "\"";
		csv << name << "," << body_str << "\n";
		csv.close();
	}
	return (0);
}

int Request::parsPart(std::vector<char>& msg, std::string& bound)
{
	std::vector<char> part;
	size_t	sep_pos;

	if (extractPart(msg, bound, part, sep_pos))
		return (1);
	std::map<std::string, std::string> headers_map = makeHeadersMap(part, sep_pos);
	if (headers_map.find("CONTENT-DISPOSITION") == headers_map.end())
		return (std::cout << "400 Error -> 12\n", setStatus("400"), 1);
	sep_pos = find_mem(part, CRLFCRLF);
	if (sep_pos == std::string::npos)
		return(std::cout << "400 error 6000" << std::endl, setStatus("400"), 1);
	part.erase(part.begin(), part.begin() + sep_pos + 4);
	if (handleContent(headers_map, part))
		return (std::cout << "error 4" << std::endl, 1);
	return (0);
}

void	Request::parsMultipart()
{
	std::string	bound;
	size_t pos = this->_headers["CONTENT-TYPE"].find("boundary=");
	bound = this->_headers["CONTENT-TYPE"].substr(pos + 9);
	removeQuotes(bound);
	bound = "--" + bound;
	pos = find_mem(this->_rcv_binary, CRLFCRLF);
	if (pos == std::string::npos)
		return;
	this->_rcv_binary.erase(this->_rcv_binary.begin(), this->_rcv_binary.begin() + pos);
	while (pos != std::string::npos)
	{
		if (parsPart(this->_rcv_binary, bound))
			break;
		pos = find_mem(this->_rcv_binary, bound);
	}
	this->_body_len = this->_count - this->_request_line_len - this->_headers_len;
}

void Request::parsCookie()
{
	if (_headers.find("COOKIE") == _headers.end())
		return;
	std::string cookie_header = _headers["COOKIE"];
	std::istringstream ss(cookie_header);
	std::string token, key, value;
	size_t pos;
	while (std::getline(ss, token, ';'))
	{
		pos = token.find('=');
		if (pos == std::string::npos)
			continue;
		key = trim_white_spaces(token.substr(0, pos));
		value = trim_white_spaces(token.substr(pos + 1));
		this->_cookies[key] = value;
	}
	this->_session_id = _cookies.begin()->first;
	// std::cout << "this->_session_id= " << this->_session_id << std::endl;
	for(std::map<std::string, std::string>::iterator it = _cookies.begin(); it != _cookies.end(); ++it)
		_server_sessions[this->_session_id][it->first] = it->second;
	// std::cout  << BOLD << CYAN << "Cookies= " << RESET;
	// for (std::map<std::string, std::string>::iterator it = _cookies.begin(); it != _cookies.end(); ++ it)
	// 	std::cout << CYAN << it->first << RESET << " -> " << CYAN << it->second << RESET << std::endl;
	// std::cout << BOLD << PURPLE << "Sessions=\n\n" << RESET;
	// for(std::map<std::string, std::map<std::string, std::string> >::iterator it1 = _server_sessions.begin(); it1 != _server_sessions.end(); ++it1)
	// {
	// 	std::cout  << BOLD << GREEN << "Id= " << it1->first << RESET << std::endl;
	// 	for(std::map<std::string, std::string>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
	// 		std::cout << GREEN << it2->first << RESET << " -> " << GREEN << it2->second << RESET << std::endl;
	// }
}