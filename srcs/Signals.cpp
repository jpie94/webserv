#include "Signals.hpp"

void	sigIntHandler(int signal)
{
	if (signal == SIGINT)
	{
		Webserv ws;
		ws.clean_close();
		throw std::runtime_error(std::string("CTRL + C caught!"));
	}
}

void	sigQuitHandler(int signal)
{
	if (signal == SIGQUIT)
	{
		Webserv ws;
		ws.clean_close();
		throw std::runtime_error(std::string("CTRL + \\ caught!"));
	}
}