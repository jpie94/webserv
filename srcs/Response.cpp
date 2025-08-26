/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qsomarri <qsomarri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:06 by qsomarri          #+#    #+#             */
/*   Updated: 2025/08/26 18:32:20 by qsomarri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

std::map<std::string, std::string>	makeTypesMap();
std::map<std::string, std::string> Response::_types = makeTypesMap();

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/
Response::Response() : Request(), _response(), _fileName(), _responseBody(), _autoIndex()//rm autoIndex
{
}

Response::Response(const Response& src)
{
	*this = src;
}

Response&	Response::operator=(const Response& rhs)
{
	if (this != &rhs)
	{
		this->_response = rhs._response;
		this->_fileName = rhs._fileName;
		this->_responseBody = rhs._responseBody;
	}
	return (*this);
}

Response::Response(Request& request) : Request(request), _response(), _fileName(), _responseBody()
{
	this->_autoIndex = 0;
}

Response::~Response() {}

/*****************	CLASS UTILS	*******************/
std::map<std::string, std::string> makeTypesMap()
{
	std::map<std::string, std::string>	types;

	types[".aac"]   = "audio/aac";
	types[".abw"]   = "application/x-abiword";
	types[".apng"]  = "image/apng";
	types[".arc"]   = "application/x-freearc";
	types[".avif"]  = "image/avif";
	types[".avi"]   = "video/x-msvideo";
	types[".azw"]   = "application/vnd.amazon.ebook";
	types[".bin"]   = "application/octet-stream";
	types[".bmp"]   = "image/bmp";
	types[".bz"]    = "application/x-bzip";
	types[".bz2"]   = "application/x-bzip2";
	types[".cda"]   = "application/x-cdf";
	types[".csh"]   = "application/x-csh";
	types[".css"]   = "text/css";
	types[".csv"]   = "text/csv";
	types[".doc"]   = "application/msword";
	types[".docx"]  = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	types[".eot"]   = "application/vnd.ms-fontobject";
	types[".epub"]  = "application/epub+zip";
	types[".gz"]    = "application/gzip";
	types[".gif"]   = "image/gif";
	types[".htm"]   = "text/html";
	types[".html"]  = "text/html";
	types[".ico"]   = "image/vnd.microsoft.icon";
	types[".ics"]   = "text/calendar";
	types[".jar"]   = "application/java-archive";
	types[".jpeg"]  = "image/jpeg";
	types[".jpg"]   = "image/jpeg";
	types[".js"]    = "text/javascript";
	types[".json"]  = "application/json";
	types[".jsonld"] = "application/ld+json";
	types[".md"]    = "text/markdown";
	types[".mid"]   = "audio/midi";
	types[".midi"]  = "audio/midi";
	types[".mjs"]   = "text/javascript";
	types[".mp3"]   = "audio/mpeg";
	types[".mp4"]   = "video/mp4";
	types[".mpeg"]  = "video/mpeg";
	types[".mpkg"]  = "application/vnd.apple.installer+xml";
	types[".odp"]   = "application/vnd.oasis.opendocument.presentation";
	types[".ods"]   = "application/vnd.oasis.opendocument.spreadsheet";
	types[".odt"]   = "application/vnd.oasis.opendocument.text";
	types[".oga"]   = "audio/ogg";
	types[".ogv"]   = "video/ogg";
	types[".ogx"]   = "application/ogg";
	types[".opus"]  = "audio/ogg";
	types[".otf"]   = "font/otf";
	types[".png"]   = "image/png";
	types[".pdf"]   = "application/pdf";
	types[".php"]   = "application/x-httpd-php";
	types[".ppt"]   = "application/vnd.ms-powerpoint";
	types[".pptx"]  = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	types[".rar"]   = "application/vnd.rar";
	types[".rtf"]   = "application/rtf";
	types[".sh"]    = "application/x-sh";
	types[".svg"]   = "image/svg+xml";
	types[".tar"]   = "application/x-tar";
	types[".tif"]   = "image/tiff";
	types[".tiff"]  = "image/tiff";
	types[".ts"]    = "video/mp2t";
	types[".ttf"]   = "font/ttf";
	types[".txt"]   = "text/plain";
	types[".vsd"]   = "application/vnd.visio";
	types[".wav"]   = "audio/wav";
	types[".weba"]  = "audio/webm";
	types[".webm"]  = "video/webm";
	types[".webmanifest"] = "application/manifest+json";
	types[".webp"]  = "image/webp";
	types[".woff"]  = "font/woff";
	types[".woff2"] = "font/woff2";
	types[".xhtml"] = "application/xhtml+xml";
	types[".xls"]   = "application/vnd.ms-excel";
	types[".xlsx"]  = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	types[".xml"]   = "application/xml";
	types[".xul"]   = "application/vnd.mozilla.xul+xml";
	types[".zip"]   = "application/zip";
	types[".3gp"]   = "video/3gpp";
	types[".3g2"]   = "video/3gpp2";
	types[".7z"]    = "application/x-7z-compressed";

	return types;
}

std::string	Response::getFileExt(std::string value) const
{
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		if (it->second == value)
			return (it->first);
	}
	return ("");
}

/*****************	MEMBER		*******************/
std::string	Response::getTime() const
{
	time_t	rawtime;

	std::time(&rawtime);
	std::tm* gmt = std::gmtime(&rawtime);
	std::string str = std::asctime(gmt);
	return (str.erase(str.find_last_not_of("\n") + 1) + " GMT");
}

std::string	Response::getFileType()
{
	size_t	pos = this->_fileName.find(".");

	if (pos != std::string::npos)
		return (this->_fileName.substr(pos, this->_fileName.size() - pos));
	return ("");
}

std::string	Response::getContent_type()
{
	if (_types.find(this->getFileType()) != _types.end())
		return (_types[this->getFileType()]);
	return ("application/octet-stream");
}

void	Response::HandlePath()
{
	struct stat	path_stat;

	this->_path = SERVER_ROOT + this->_path;
	if (this->_path[0] == '/')
		this->_path = this->_path.substr(1);
	// std::cout << "1- this->_path= " << this->_path << '\n';
	if (stat(this->_path.c_str(), &path_stat) != 0)
			return(setStatus("404"));
	if (S_ISDIR(path_stat.st_mode))
	{
		if (this->_autoIndex && this->_methode == "GET")
			return ((void)this->autoIndex());
		if (this->_methode == "GET")
		{
			std::string	str(this->_path + "index.html");
			int	status = open(str.c_str(), O_RDONLY);
			if (status < 0)
				return(setStatus("403"));
			this->_path += "index.html";
			if (close(status) < 0)
				return(setStatus("500"));
		}
		if (this->_methode == "POST")
		{
			this->_fileName = "newFile";//check for unique file name
			if (this->_headers.find("CONTENT-TYPE") != this->_headers.end())
				this->_fileName += getFileExt(this->_headers["CONTENT-TYPE"]);
			this->_path += this->_fileName;//check '/'
		}
	}
	std::ifstream	ifs(this->_path.c_str(), std::ifstream::in);
	if (ifs.fail() || !ifs.is_open())
		return(setStatus("404"));
	size_t	pos = this->_path.rfind("/");
	if (pos != std::string::npos)
		this->_fileName = this->_path.substr(pos + 1, this->_path.size() - pos - 1);
	// std::cout << "final path= " << this->_path << '\n';
	// std::cout << "file name= " << this->_fileName << "\n\n";
}

void	Response::readFile()
{
	std::ostringstream	os;
	std::ifstream	file(this->_path.c_str(), std::ios::in|std::ios::binary);

	if (file.fail())
		return(setStatus("500"), setErrorPage());
	os << file.rdbuf();
	this->_responseBody = os.str() + CRLF;
}

void	Response::getMethode()
{
	if (this->_responseStatus != "200")
		return (std::cout << "ok1" << std::endl, setErrorPage());
	std::cout << "GET methode called\n";
	this->readFile();
	if (this->_responseStatus == "200")
		return (setResponse());
	else
		return (std::cout << "ok2" << std::endl, setErrorPage());
}

void	Response::postMethode()
{
	std::string	status;
	struct stat	path_stat;

	if (this->_responseStatus != "200")
		return (setErrorPage());
	std::cout << "POST methode called\n";
	if (this->_body.empty())
		return (setStatus("400"), setErrorPage());
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
	if (!ofs.is_open())
		return(setStatus("500"), setErrorPage());
	ofs << this->_body;
	//ofs.close();
}

void	Response::deleteMethode()
{
	if (this->_responseStatus != "200")
		return (setErrorPage());
	if (std::remove(this->_path.c_str()))
		return (setStatus("403"), setErrorPage());
	std::cout << "DELETE methode called on: " << this->_path << "\n";
	this->_responseBody += "File: " + this->_fileName + " deleted" + CRLF;
	setResponse();
}

void	Response::autoIndex()
{
	DIR		*dir;
	struct dirent	*ent;
	std::string	index_page = "<!DOCTYPE html>\n<html>\n<head>\n<title>Page Title</title>\n</head>\n<body>\n\n<h1>Index of ";
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
		this->_responseBody = index_page;
		std::cout << this->_responseBody << std::endl;
		closedir(dir);
	}
	else
		Webserv::throw_error("403: forbidden");
}

void	Response::setResponse()
{
	this->_response += "HTTP/1.1 " + this->_responseStatus;
	if (this->_responseStatus == "200")
		this->_response += " OK";
	else if (this->_responseStatus == "201")
		this->_response += " Created";
	this->_response += CRLF;
	if (this->_methode == "GET")
	{
		this->_response += "Server: Webserv";
		this->_response += CRLF;
		this->_response += "Date: " + this->getTime() + CRLF;
		this->_response += "Content-type: " + this->getContent_type() + CRLF;
		this->_response += "Content-Length: " + i_to_string(this->_responseBody.size()) + CRLFCRLF;
	}
	if (this->_methode == "POST")
	{
		this->_response += "Server: Webserv";
		this->_response += CRLF;
		this->_response += "Date: " + getTime() + CRLF;
		this->_response += "Content-type: " + getContent_type();//check value
		this->_response += "Content-Length: " + i_to_string(_responseBody.size()) + CRLFCRLF;
	}
	if (this->_methode == "DELETE")
	{
		this->_response += CRLF;
		this->_response += "Date: " + this->getTime() + CRLF;
		this->_response +="Content-type: " + this->getContent_type() + CRLF;
		this->_response += "Content-Length: " + i_to_string(this->_responseBody.size()) + CRLFCRLF;
	}
	this->_response += this->_responseBody;
}


void	Response::callMethode()
{
	std::string	methodes[3] = {"GET", "POST", "DELETE"};
	void	(Response::*f[])(void) = {&Response::getMethode, &Response::postMethode, &Response::deleteMethode};

	if (this->_responseStatus != "200")
		return(setErrorPage());
	HandlePath();
	for (int i = 0; i < 3; ++i)
		if (!methodes[i].compare(this->_methode))
			(void)((this->*f[i])());
	if (this->_responseStatus == "200" || this->_responseStatus == "201")
		printResponse();
}
void	Response::setErrorPage()
{
	std::string		InternalError500;
	std::ostringstream	os;
	std::string		target("root/error/" + this->_responseStatus + ".html");
	std::ifstream		file(target.c_str());

	if (file.fail())
	{
		this->_responseBody = "<p style=\"text-align: center;\"><strong>500 Internal Server Error</strong></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">___________________________________________________________________________________________</span></p> \
		<p style=\"text-align: center;\"><span style=\"font-size: 10px;\">webserv</span></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p> \
		<p style=\"text-align: center;\"><br></p>";
	}
	else
	{
		os << file.rdbuf();
		this->_responseBody = os.str();
	}
	std::cout << "Error page to send : " << this->_responseBody;
}

void	Response::printResponse() const
{
	std::cout << "Response=\n" << this->_response << '\n';
}