/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response2.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/04 18:09:52 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/05 13:51:45 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

int Response::HandlePath()
{
	struct stat path_stat;
	int			status;
	std::memset(&path_stat, 0, sizeof(path_stat));
	status = stat(this->_path.c_str(), &path_stat);
	if (status && this->_methode.compare("POST"))
		return (setStatus("404"), 0);
	if ((access(this->_path.c_str(), R_OK | W_OK) && (S_ISDIR(path_stat.st_mode) || S_ISREG(path_stat.st_mode))))
		return (setStatus("403"), 0);
	if (!status && S_ISDIR(path_stat.st_mode))
	{
		if (this->_autoIndex == "on" && this->_methode == "GET")
			return (this->autoIndex(), 1);
		if (this->_methode == "GET")
		{
			std::string str(this->_path + "index.html");
			std::ifstream ifs(str.c_str());
			if (ifs.fail())
				return (setStatus("404"), 0);
			this->_path += "index.html";
		}
		if (this->_methode == "POST")
		{
			this->_fileName = "newFile";
			if (this->_headers.find("CONTENT-TYPE") != this->_headers.end())
				this->_fileName += getFileExt(this->_headers["CONTENT-TYPE"]);
			if (stat(this->_fileName.c_str(), &path_stat))
			{
				size_t pos = this->_fileName.rfind('.');
				this->_fileName = this->_fileName.substr(0, pos);
				this->_fileName += getTime() + getFileExt(this->_headers["CONTENT-TYPE"]);
			}
			this->_path += this->_fileName;
		}
	}
	else if (!status && this->_methode.compare("POST"))
	{
		std::ifstream ifs(this->_path.c_str(), std::ifstream::in);
		if (ifs.fail() || !ifs.is_open())
			return (setStatus("403"), 0);
	}
	else if (!status && !this->_methode.compare("POST"))
	{
		std::ofstream ofs(this->_path.c_str(), std::ofstream::out);
		if (ofs.fail() || !ofs.is_open())
			return (setStatus("403"), 0);
	}
	size_t pos = this->_path.rfind("/");
	if (pos != std::string::npos)
		this->_fileName = this->_path.substr(pos + 1, this->_path.size() - pos - 1);
	return (0);
}

void Response::readFile()
{
	std::ostringstream os;
	std::ifstream file(this->_path.c_str(), std::ios::in | std::ios::binary);

	if (file.fail())
		return ((void)(std::cout << "500 Error -> 1\n"),setStatus("500"), setErrorPage());
	os << file.rdbuf();
	this->_responseBody = os.str() + CRLF;
}

void Response::getMethode()
{
	this->readFile();
	if (this->_responseStatus == "200")
		return (setResponse());
	else
		return (setErrorPage());
}

void Response::postMethode()
{
	std::string status;
	struct stat path_stat;

	if (this->_body.empty())//check if useless
		return ((void)(std::cout << "400 Error -> 7\n"), setStatus("400"), setErrorPage());
	if (stat(this->_path.c_str(), &path_stat) != 0)
	{
		setStatus("201");
		this->_responseBody = "Resource succesfully created";
	}
	else
	{
		setStatus("200");
		this->_responseBody = "Resource succedfully upadated";
	}
	this->_responseBody += CRLF;
	std::ofstream ofs(this->_path.c_str(), std::ios::out | std::ios::binary);
	// std::cout << "this->_fileName= " << this->_fileName << std::endl;
	// std::cout << "this->_path= " << this->_path << std::endl;
	if (!ofs.is_open() || ofs.fail())
		return ((void)(std::cout << "500 Error -> 2\n"),setStatus("500"), setErrorPage());
	ofs << this->_body;
	// ofs.close();
	setResponse();
}

void Response::deleteMethode()
{
	if (std::remove(this->_path.c_str()))
	{
		std::cout << "here it's forbiden\n";
		return (setStatus("403"), setErrorPage());
	}
	this->_responseBody += "File: " + this->_fileName + " deleted" + CRLF;
	setResponse();
}

void Response::autoIndex()
{
	DIR *dir;
	struct dirent *ent;
	std::string index_page = "<!DOCTYPE html>\n<html>\n<head>\n<title>Page Title</title>\n</head>\n<body>\n\n<h1>Index of ";
	index_page += this->_path + "</h1>";

	if ((dir = opendir(this->_path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] == '.')
				continue;
			index_page += ent->d_name;
			index_page += "<p>";
		}

		index_page += "</p>\n</body>\n</html>\n";
		if (closedir(dir) < 0)
			return ((void)(std::cout << "500 Error -> 3\n"),setStatus("500"), setErrorPage());
	}
	else
		return (setStatus("403"), setErrorPage());
	this->_fileName = "autoIndex.html";
	this->_responseBody = index_page;
	setResponse();
}

void Response::setRedirect()
{
	this->_response_msg += "HTTP/1.1 " + this->_responseStatus;
	this->_response_msg += " Found ";
	this->_response_msg += "\r\nServer: Webserv\r\n";
	this->_response_msg += "Date: " + this->getTimeStr() + CRLF;
	this->_response_msg += "Content-length: 0\r\n";
	this->_response_msg += "Location: " + this->_config["return"] + CRLFCRLF;
}

void Response::setResponse()
{
	this->_response_msg += "HTTP/1.1 " + this->_responseStatus;
	if (this->_responseStatus == "200")
		this->_response_msg += " OK";
	if (this->_responseStatus == "201")
		this->_response_msg += " Created";
	this->_response_msg += "\r\nServer: Webserv\r\n";
	this->_response_msg += "Date: " + this->getTimeStr() + CRLF;
	this->_response_msg += "Content-type: " + this->getContent_type() + CRLF;
	this->_response_msg += "Content-Length: " + i_to_string(this->_responseBody.size()) + CRLFCRLF;
	this->_response_msg += this->_responseBody;
}

void Response::callMethode()
{
	std::string methodes[3] = {"GET", "POST", "DELETE"};
	void (Response::*f[])(void) = {&Response::getMethode, &Response::postMethode, &Response::deleteMethode};
	if (this->_responseStatus == "302")
		return (setRedirect());
	if (this->_responseStatus != "200")
		return (setErrorPage());
	if (HandlePath())
		return;
	if (this->_responseStatus != "200")
		return (setErrorPage());
	for (int i = 0; i < 3; ++i)
		if (!methodes[i].compare(this->_methode))
			(void)((this->*f[i])());
}
void Response::setErrorPage()
{
	std::ostringstream os;
	std::string target(this->_ogRoot + "/error/" + this->_responseStatus + ".html");
	std::ifstream file(target.c_str());

	if (file.fail())
	{
		this->_responseBody = "<p style=\"text-align: center;\"><strong>500 Internal Server Error</strong></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">___________________________________________________________________________________________</span></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">webserv</span></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p>";
		this->_fileName = "500.html";
	}
	else
	{
		os << file.rdbuf();
		this->_responseBody = os.str();
		this->_fileName = target;
	}
	setResponse();
}
