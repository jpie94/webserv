/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <jpiech@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:06 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/29 11:01:42 by jpiech           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"
#include "Request.hpp"
#include "Client.hpp"

/*****************	CANONICAL + PARAMETRIC CONSTRUCTOR 	*******************/

CGI::CGI() : Request(), _varEnv(), _In(), _Out(), _PID() {}

CGI::CGI(const CGI &src)
{
	*this = src;
}

CGI &CGI::operator=(const CGI &rhs)
{
	if (this != &rhs)
	{
		for(int i = 0; i < 18; i++)
			this->_varEnv[i] = rhs._varEnv[i];
		this->_In = rhs._In;
		this->_Out = rhs._Out;
		this->_PID = rhs._PID;
	}
	return (*this);
}

CGI::CGI(Request &request) : Request(request)
{
	fillVarEnv();
	newProcess();
	int i = 0;
	while (this->_varEnv[i])
	{
		std::cout << this->_varEnv[i] <<std::endl;
		i++;
	}
}

CGI::~CGI() {}

/*****************	CLASS UTILS	*******************/

void	CGI::fillVarEnv()
{
	std::vector<std::string> tempEnv;
	tempEnv.push_back("AUTH_TYPE=");
	tempEnv.push_back("CONTENT_LENGTH=" + this->_headers["CONTENT-LENGTH"]);
	tempEnv.push_back("CONTENT_TYPE=" + this->_headers["CONTENT-TYPE"]);
	tempEnv.push_back("GATEWAY_INTERFACE=CGI/1.1");
	tempEnv.push_back("PATH_INFO=" + this->_CGI_pathInfo);
	if(this->_CGI_pathInfo != "")
		tempEnv.push_back("PATH_TRANSLATED=" + _ogRoot + "/" + this->_CGI_pathInfo);
	else
		tempEnv.push_back("PATH_TRANSLATED=");
	tempEnv.push_back("QUERY_STRING=" + this->_CGI_querry);
	tempEnv.push_back("REMOTE_ADDR=" + this->_config["server_name"]); // j arrive pas a recuperer l ip a partir de la socket, j ai besoin de inet_ntop et cest pas une fonction autorisee
	tempEnv.push_back("REMOTE_HOST="); // je peux pas le recuperer donc toujours set a null
	tempEnv.push_back("REMOTE_IDENT="); //Depend du AUTH_TYPE, pas gere pas notre serveur
	tempEnv.push_back("REMOTE_USER="); //Depend du AUTH_TYPE, pas gere pas notre serveur
	tempEnv.push_back("REQUEST_METHOD=" + this->_methode); 
	tempEnv.push_back("SCRIPT_NAME=" + this->_CGI_bin_path + this->_CGI_script);
	tempEnv.push_back("SERVER_NAME=" + this->_config["server_name"]);
	tempEnv.push_back("SERVER_PORT=" + this->_config["listen"]);
	tempEnv.push_back("SERVER_PROTOCOL= " + this->_protocol);
	tempEnv.push_back("SERVER_SOFTWARE=webserv");
	for(size_t i = 0; i < tempEnv.size(); i++)
	{
		this->_varEnv[i] = new char[tempEnv[i].length() + 1];
		std::memcpy(this->_varEnv[i], tempEnv[i].c_str(), tempEnv[i].length() + 1);
	}
	this->_varEnv[tempEnv.size()] = NULL;	
}

void	CGI::newProcess()
{
	int PtoC[2];
	int CtoP[2];
	if (pipe(PtoC))
		throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : pipe PtoC failed : ") + std::strerror(errno)).c_str());
	if (pipe(CtoP))
		throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : pipe CtoP failed : ") + std::strerror(errno)).c_str());
	int pid = fork();
	if (pid == -1)
		throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : fork failed : ") + std::strerror(errno)).c_str());
	this->_In = PtoC[1];
	this->_Out = CtoP[0];
	this->_PID = pid;
	if (pid == 0)
	{		
		if (close(PtoC[1]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close PtoC[1] failed in child: ") + std::strerror(errno)).c_str());
		if (close(CtoP[0]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close CtoP[0] failed in child: ") + std::strerror(errno)).c_str());
		dup2(PtoC[0], STDIN_FILENO);
		dup2(CtoP[1], STDOUT_FILENO);
		if (close(PtoC[0]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close PtoC[1] failed in child: ") + std::strerror(errno)).c_str());
		if (close(CtoP[1]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close CtoP[1] failed in child: ") + std::strerror(errno)).c_str());
		executeCGI();
	}
	else
	{
		if (close(PtoC[0]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close PtoC[0] failed in parent: ") + std::strerror(errno)).c_str());
		if (close(CtoP[1]) < 0)
			throw std::runtime_error(std::string(std::string("Error in CGI::newProcess : close CtoP[1] failed in parent: ") + std::strerror(errno)).c_str());
	}
}

void	CGI::executeCGI()
{
	std::string script = this->_CGI_bin_path + this->_CGI_script;
	char * execArg[] = {const_cast<char *>(this->_CGIinterpret.c_str()), const_cast<char *>(script.c_str()), NULL};
	execve(execArg[0], execArg, this->_varEnv);
}
/*****************	GETTERS		*******************/

int CGI::get_FD_In () const
{
	return(this->_In);	
}

int	CGI::get_FD_Out () const
{
	return(this->_Out);	
}

int	CGI::get_PID () const
{
	return(this->_PID);	
}

void	CGI::clear_CGI() const
{
	for(int i = 0; i < 18; i++)
	{
		if(this->_varEnv[i] != NULL)
			delete[] this->_varEnv[i];
	}
	// if (close (this->_In) < 0)
	// 	throw std::runtime_error(std::string(std::string("Error in clear_CGI : close _In failed : ") + std::strerror(errno)).c_str());
	if (close (this->_Out) < 0)
		throw std::runtime_error(std::string(std::string("Error in clear_CGI : close _Out failed : ") + std::strerror(errno)).c_str());
}