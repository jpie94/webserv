#include "Webserv.hpp"
#include "Request.hpp"
#include "Signals.hpp"

int	main(int argc, char *argv[])
{
	try
	{
		std::signal(SIGINT, sigIntHandler);
		std::signal(SIGQUIT, sigQuitHandler);
		if (argc == 1)
		{
			Webserv ws;
			ws.runWebserv();
		}
		else if (argc == 2)
		{
			Webserv ws(argv[1]);
			ws.runWebserv();	
		}
		else
			throw std::invalid_argument("Wrong number of arguments, must be 1 or 0");
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}

/* Request Test main */

// int main(int argc, char** argv)
// {
// 	if (argc != 2)
// 		return (1);
// 	try
// 	{
// 		Request r(argv[1]);
// 		r.parsRequest();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}
// 	return (0);
// }