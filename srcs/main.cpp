#include "Webserv.hpp"
#include "Request.hpp"
#include <csignal>

// void	sigIntHandler(int signal)
// {
// 	if (signal == SIGINT)
// 	{
// 		Webserv ws;
// 		for (unsigned int i = 0; i < ws.getPfds().size(); ++i)
// 		{
// 			if (ws.getPfds()[i].fd > 0)
// 			{
// 			if (close(ws.getPfds()[i].fd) < 0)
// 				ws.throw_error("close");
// 			else
// 				ws.getPfds()[i].fd = -1;
// 			}
// 		}
// 		throw std::runtime_error(std::string("CTRL + C caught!"));
// 	}
// }

// void	sigQuitHandler(int signal)
// {
// 	if (signal == SIGQUIT)
// 	{
// 		Webserv ws;
// 		for (unsigned int i = 0; i < ws.getPfds().size(); ++i)
// 		{
// 			if (ws.getPfds()[i].fd > 0)
// 			{
// 			if (close(ws.getPfds()[i].fd) < 0)
// 				ws.throw_error("close");
// 			else
// 				ws.getPfds()[i].fd = -1;
// 			}
// 		}
// 		throw std::runtime_error(std::string("CTRL + \\ caught!"));
// 	}
// }

// int	main()
// {
// 	try
// 	{
// 		Webserv ws;
// 		std::signal(SIGINT, sigIntHandler);
// 		std::signal(SIGQUIT, sigQuitHandler);
// 		ws.runWebserv();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}
// 	return (0);
// }

/* Request Test main */

int main(int argc, char** argv)
{
	if (argc != 2)
		return (1);
	try
	{
		Request r(argv[1]);
		r.parsRequest();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}