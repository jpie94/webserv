#include "Webserv.hpp"
#include "Request.hpp"
#include "Signals.hpp"

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
		r.handleMethode();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}