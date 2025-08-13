#include "Webserv.hpp"
#include <cstdio>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <csignal>
#include <vector>

void sigIntHandler(int signal)//webserv class
{
	if (signal == SIGINT)
		throw std::runtime_error(std::string("CTRL + C caught!"));	
}

void sigQuitHandler(int signal)//webserv class
{
	if (signal == SIGQUIT)
		throw std::runtime_error(std::string("CTRL + \\ caught!"));	
}

int	main()
{
	try
	{
		Webserv ws;
		std::signal(SIGINT, sigIntHandler);
		std::signal(SIGQUIT, sigQuitHandler);
		ws.make_listening_socket();
		ws.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
