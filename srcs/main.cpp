#include "Webserv.hpp"
#include <csignal>

void	sigIntHandler(int signal)
{
	if (signal == SIGINT)
	
		throw std::runtime_error(std::string("CTRL + C caught!"));	
}

void	sigQuitHandler(int signal)
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
		ws.runWebserv();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
