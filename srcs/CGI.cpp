/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpiech <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 14:16:06 by qsomarri          #+#    #+#             */
/*   Updated: 2025/09/18 15:41:00 by jpiech           ###   ########.fr       */
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
		for (int i = 0; i < 18; i++)
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
		//new pipe dabord, fork ensuite, puis dup2 dans l enfant;
}

CGI::~CGI() {}

/*****************	CLASS UTILS	*******************/

void	CGI::fillVarEnv()
{
	this->_varEnv[0] = "AUTH_TYPE =";
	this->_varEnv[1] = const_cast<char *> (std::string("CONTENT_LENGTH = " + this->_headers["CONTENT_LENGTH"]).c_str());
	this->_varEnv[2] = const_cast<char *> (std::string("CONTENT_TYPE = " + this->_headers["CONTENT_TYPE"]).c_str());
	this->_varEnv[3] = const_cast<char *> (std::string("GATEWAY_INTERFACE = CGI/1.1").c_str());
	this->_varEnv[4] = const_cast<char *> (std::string("PATH_INFO = " + this->_path.substr(this->_path.rfind("/"))).c_str());
	this->_varEnv[5] = const_cast<char *> (std::string("PATH_TRANSLATED = " + _ogRoot + this->_path.substr(this->_path.rfind("/"))).c_str());
	this->_varEnv[6] = const_cast<char *> (std::string("QUERY_STRING = " + this->_path.substr(this->_path.rfind("?"))).c_str());
	this->_varEnv[7] = const_cast<char *> (std::string("REMOTE_ADDR = " + this->_headers["REMOTE_ADDR"]).c_str());
	this->_varEnv[8] = const_cast<char *> (std::string("REMOTE_HOST = " + this->_headers["REMOTE_HOST"]).c_str());
	this->_varEnv[9] = const_cast<char *> (std::string("REMOTE_IDENT = " + this->_headers["REMOTE_IDENT"]).c_str());
	this->_varEnv[10] = const_cast<char *> (std::string("REMOTE_USER = " + this->_headers["REMOTE_USER"]).c_str());
	this->_varEnv[11] = const_cast<char *> (std::string("REQUEST_METHOD = " + this->_methode).c_str());
	this->_varEnv[12] = const_cast<char *> (std::string("SCRIPT_NAME = " + this->_path).c_str());
	this->_varEnv[13] = const_cast<char *> (std::string("SERVER_NAME = " + this->_config["server_name"]).c_str());
	this->_varEnv[14] = const_cast<char *> (std::string("SERVER_PORT = " + this->_config["listen"]).c_str());
	this->_varEnv[15] = const_cast<char *> (std::string("SERVER_PROTOCOL = " + this->_protocol).c_str());
	this->_varEnv[16] = "webserv";
	this->_varEnv[17] = NULL;
}

void	CGI::newProcess()
{
	int PtoC[2];
	int CtoP[2];
	pipe(PtoC);
	pipe(CtoP);
	int pid = fork(); // renvoie le PID de l enfant qu il vient de creer 
	//stocker le PID du process enfant 
	this->_In = PtoC[1];
	this->_Out = CtoP[0];
	this->_PID = pid;
	if (pid == 0)
	{		
		close(PtoC[1]);
		close(CtoP[0]);
		dup2(PtoC[0], STDIN_FILENO);
		dup2(CtoP[1], STDOUT_FILENO);
		close(PtoC[0]);
		close(CtoP[1]);
		executeCGI();
	}
	else
	{
		close(PtoC[0]);
		close(CtoP[1]);
	}
}

void	CGI::executeCGI()
{
	char * args[2];
	args[0] = const_cast<char *>(this->_path.c_str());
	args[1] = NULL;
	execve(this->_CGIinterpret.c_str(), args, this->_varEnv);
}
/*****************	GETTERS		*******************/

int CGI::get_FD_In ()
{
	return(this->_In);	
}

int CGI::get_FD_Out ()
{
	return(this->_Out);	
}

int CGI::get_PID ()
{
	return(this->_PID);	
}
