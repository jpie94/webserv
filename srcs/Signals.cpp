#include "Signals.hpp"

void	sigIntHandler(int signal)
{
	if (signal == SIGINT)
	{
		Webserv ws;
		ws.throw_error("CTRL + C caught!");
	}
}

void	sigQuitHandler(int signal)
{
	if (signal == SIGQUIT)
	{
		Webserv ws;
		ws.throw_error("CTRL + \\ caught!");
	}
}