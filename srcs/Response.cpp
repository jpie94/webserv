/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:06 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/03 17:55:34 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Request.hpp"
#include "Client.hpp"

std::map<std::string, std::string> makeTypesMap();
std::map<std::string, std::string> Response::_types = makeTypesMap();

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

Response::Response() : Request(), _response_msg(), _fileName(), _responseBody(), _autoIndex() {} // rm autoIndex

Response::Response(const Response &src)
{
	*this = src;
}

Response &Response::operator=(const Response &rhs)
{
	if (this != &rhs)
	{
		this->_response_msg = rhs._response_msg;
		this->_fileName = rhs._fileName;
		this->_responseBody = rhs._responseBody;
	}
	return (*this);
}

Response::Response(Request &request) : Request(request), _fileName(), _responseBody()
{
	this->_autoIndex = 0;
}

Response::~Response() {}

/*****************	CLASS UTILS	*******************/

std::map<std::string, std::string> makeTypesMap()
{
	std::map<std::string, std::string> types;

	types[".aac"] = "audio/aac";
	types[".abw"] = "application/x-abiword";
	types[".apng"] = "image/apng";
	types[".arc"] = "application/x-freearc";
	types[".avif"] = "image/avif";
	types[".avi"] = "video/x-msvideo";
	types[".azw"] = "application/vnd.amazon.ebook";
	types[".bin"] = "application/octet-stream";
	types[".bmp"] = "image/bmp";
	types[".bz"] = "application/x-bzip";
	types[".bz2"] = "application/x-bzip2";
	types[".cda"] = "application/x-cdf";
	types[".csh"] = "application/x-csh";
	types[".css"] = "text/css";
	types[".csv"] = "text/csv";
	types[".doc"] = "application/msword";
	types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	types[".eot"] = "application/vnd.ms-fontobject";
	types[".epub"] = "application/epub+zip";
	types[".gz"] = "application/gzip";
	types[".gif"] = "image/gif";
	types[".htm"] = "text/html";
	types[".html"] = "text/html";
	types[".ico"] = "image/vnd.microsoft.icon";
	types[".ics"] = "text/calendar";
	types[".jar"] = "application/java-archive";
	types[".jpeg"] = "image/jpeg";
	types[".jpg"] = "image/jpeg";
	types[".js"] = "text/javascript";
	types[".json"] = "application/json";
	types[".jsonld"] = "application/ld+json";
	types[".md"] = "text/markdown";
	types[".mid"] = "audio/midi";
	types[".midi"] = "audio/midi";
	types[".mjs"] = "text/javascript";
	types[".mp3"] = "audio/mpeg";
	types[".mp4"] = "video/mp4";
	types[".mpeg"] = "video/mpeg";
	types[".mpkg"] = "application/vnd.apple.installer+xml";
	types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	types[".odt"] = "application/vnd.oasis.opendocument.text";
	types[".oga"] = "audio/ogg";
	types[".ogv"] = "video/ogg";
	types[".ogx"] = "application/ogg";
	types[".opus"] = "audio/ogg";
	types[".otf"] = "font/otf";
	types[".png"] = "image/png";
	types[".pdf"] = "application/pdf";
	types[".php"] = "application/x-httpd-php";
	types[".ppt"] = "application/vnd.ms-powerpoint";
	types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	types[".rar"] = "application/vnd.rar";
	types[".rtf"] = "application/rtf";
	types[".sh"] = "application/x-sh";
	types[".svg"] = "image/svg+xml";
	types[".tar"] = "application/x-tar";
	types[".tif"] = "image/tiff";
	types[".tiff"] = "image/tiff";
	types[".ts"] = "video/mp2t";
	types[".ttf"] = "font/ttf";
	types[".txt"] = "text/plain";
	types[".vsd"] = "application/vnd.visio";
	types[".wav"] = "audio/wav";
	types[".weba"] = "audio/webm";
	types[".webm"] = "video/webm";
	types[".webmanifest"] = "application/manifest+json";
	types[".webp"] = "image/webp";
	types[".woff"] = "font/woff";
	types[".woff2"] = "font/woff2";
	types[".xhtml"] = "application/xhtml+xml";
	types[".xls"] = "application/vnd.ms-excel";
	types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	types[".xml"] = "application/xml";
	types[".xul"] = "application/vnd.mozilla.xul+xml";
	types[".zip"] = "application/zip";
	types[".3gp"] = "video/3gpp";
	types[".3g2"] = "video/3gpp2";
	types[".7z"] = "application/x-7z-compressed";

	return types;
}

std::string Response::getFileExt(std::string value) const
{
	for (std::map<std::string, std::string>::const_iterator it = _types.begin(); it != _types.end(); ++it)
		if (it->second == value)
			return (it->first);
	return ("");
}

/*****************	MEMBER		*******************/
std::string Response::getTimeStr() const
{
	time_t rawtime;

	std::time(&rawtime);
	std::tm *gmt = std::gmtime(&rawtime);
	std::string str = std::asctime(gmt);
	return (str.erase(str.find_last_not_of("\n") + 1) + " GMT");
}

std::string	Response::getTime() const
{
    std::time_t now = std::time(NULL);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;

	oss << (t->tm_year + 1900)
		<< (t->tm_mon + 1 < 10 ? "0" : "") << (t->tm_mon + 1)
		<< (t->tm_mday < 10 ? "0" : "") << t->tm_mday << "_"
		<< (t->tm_hour < 10 ? "0" : "") << t->tm_hour
		<< (t->tm_min < 10 ? "0" : "") << t->tm_min
		<< (t->tm_sec < 10 ? "0" : "") << t->tm_sec;
    return (oss.str());
}

std::string Response::getFileType()
{
	size_t pos = this->_fileName.rfind(".");

	if (pos != std::string::npos)
		return (this->_fileName.substr(pos, this->_fileName.size() - pos));
	return ("");
}

std::string Response::getContent_type()
{
	if (_types.find(this->getFileType()) != _types.end())
		return (_types[this->getFileType()]);
	return ("application/octet-stream");
}

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
		if (this->_autoIndex && this->_methode == "GET")
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
			// int i = 0;
			this->_fileName = "newFile";
			// std::cout << "headers[CONTENT-TYPE]= " << this->_headers["CONTENT-TYPE"] << std::endl;
			// std::cout << "file ext= " << getFileExt(this->_headers["CONTENT-TYPE"]) << std::endl;
			if (this->_headers.find("CONTENT-TYPE") != this->_headers.end())
				this->_fileName += getFileExt(this->_headers["CONTENT-TYPE"]);
			std::cout << "filename= " << this->_fileName << '\n';
			if (stat(this->_fileName.c_str(), &path_stat))
			{
				// std::cout << "file name= " << this->_fileName << std::endl;
				size_t pos = this->_fileName.rfind('.');
				this->_fileName = this->_fileName.substr(0, pos);
				// std::cout << "file name without ext= " << this->_fileName << std::endl;
				// std::cout << "i= " << i_to_string(i + 1) << std::endl;
				// std::cout << "headers[CONTENT-TYPE]= " << this->_headers["CONTENT-TYPE"] << std::endl;
				// std::cout << "file ext= " << getFileExt(this->_headers["CONTENT-TYPE"]) << std::endl;
				this->_fileName += getTime() + getFileExt(this->_headers["CONTENT-TYPE"]);
				std::cout << "filename= " << this->_fileName << '\n';
			}
			this->_path += this->_fileName;
			// std::cout << "new_file_name: " << this->_fileName << std::endl;
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
		return (setStatus("500"), setErrorPage());
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
	if (!ofs.is_open() || ofs.fail())
		return (setStatus("500"), setErrorPage());
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
			return (setStatus("500"), setErrorPage());
	}
	else
		return (setStatus("403"), setErrorPage());
	this->_fileName = "autoIndex.html";
	this->_responseBody = index_page;
	setResponse();
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
	std::string target("./root/error/" + this->_responseStatus + ".html");
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

std::string Response::getResponseMsg() const
{
	return (this->_response_msg);
}
