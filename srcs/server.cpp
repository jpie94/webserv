#include "Client.hpp"
#include "Webserv.hpp"
#include <cstdio>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <csignal>
#include <vector>

void	add_client_to_pollfds(std::vector<struct pollfd> &pfds, int server_fd, nfds_t &j, std::vector<Client> &client_list)//server class
{
	int socket_fd;
	socket_fd = accept(server_fd, NULL, 0);
	if (socket_fd < 0)
		throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	pfds.push_back(temp);
	Client temp1(socket_fd, (pfds.size() -1));
	client_list.push_back(temp1);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
}

void	erase_from_pollfd(std::vector<struct pollfd> &pfds, nfds_t &j)//server class
{
	if (close(pfds[j].fd) < 0)
		throw_error("close");
	pfds.erase(pfds.begin() + j--);
	//Faudra penser a mettre a jour l attribut index dans tous les clients en fonction de cet erase pour faire correspondre a la structure de pollfds
}

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
	int server_fd;
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
	close(server_fd);
	return (0);
}
