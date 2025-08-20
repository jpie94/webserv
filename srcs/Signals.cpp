#include "Signals.hpp"

void	sigIntHandler(int signal)
{
	if (signal == SIGINT)
	{
		Webserv ws;
		for (unsigned int i = 0; i < ws.getPfds().size(); ++i)
		{
			if (ws.getPfds()[i].fd > 0)
			{
			if (close(ws.getPfds()[i].fd) < 0)
				ws.throw_error("close");
			else
				ws.getPfds()[i].fd = -1;
			}
		}
		throw std::runtime_error(std::string("CTRL + C caught!"));
	}
}

void	sigQuitHandler(int signal)
{
	if (signal == SIGQUIT)
	{
		Webserv ws;
		for (unsigned int i = 0; i < ws.getPfds().size(); ++i)
		{
			if (ws.getPfds()[i].fd > 0)
			{
			if (close(ws.getPfds()[i].fd) < 0)
				ws.throw_error("close");
			else
				ws.getPfds()[i].fd = -1;
			}
		}
		
		throw std::runtime_error(std::string("CTRL + \\ caught!"));
	}
}