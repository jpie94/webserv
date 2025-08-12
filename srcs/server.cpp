#include "Client.hpp"
#include <cstdio>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <exception>
#include <sstream>
#include <csignal>
#include <vector>

#define PORT "8080"
#define IP "127.0.0.1"


void throw_error(const char* msg)
{
    std::perror(msg);
    throw std::runtime_error(msg);
}

int	make_listening_socket()
{
	struct addrinfo hint;
	struct addrinfo *addr;
	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(IP, PORT, &hint, &addr) != 0)
		throw_error("getaddrinfo");
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		throw_error("socket");
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		freeaddrinfo(addr);
		throw_error("bind");
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
		throw_error("listen");
	return (socket_fd);
}

void	add_client_to_pollfds(std::vector<struct pollfd> &pfds, int server_fd, nfds_t &j, std::vector<Client> &client_list)
{
	int socket_fd;
	socket_fd = accept(server_fd, NULL, 0);
	if (socket_fd < 0)
		throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	pfds.push_back(temp);
	Client temp1(socket_fd, j);
	client_list.push_back(temp1);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
}

void	erase_from_pollfd(std::vector<struct pollfd> &pfds, nfds_t &j)
{
	if (close(pfds[j].fd) < 0)
		throw_error("close");
	pfds.erase(pfds.begin() + j--);
	//Faudra penser a mettre a jour l attribut index dans tous les clients en fonction de cet erase pour faire correspondre a la structure de pollfds
}

void sigIntHandler(int signal)
{
	if (signal == SIGINT)
		throw std::runtime_error(std::string("CTRL + C caught!"));	
}

void sigQuitHandler(int signal)
{
	if (signal == SIGQUIT)
		throw std::runtime_error(std::string("CTRL + \\ caught!"));	
}

int	main()
{
	int server_fd;
	try
	{
		std::signal(SIGINT, sigIntHandler);
		std::signal(SIGQUIT, sigQuitHandler);
		std::vector<struct pollfd> pfds;
		std::vector<Client> client_list;
		int status, server_fd = make_listening_socket();
		if (server_fd < 0)
			return (1);
		struct pollfd temp = {server_fd, POLLIN, 0};
		pfds.push_back(temp);
		while(1)
		{
			status = poll(pfds.data(), pfds.size(), 2000);
			if (status < 0)
				throw_error("poll");
			if (status == 0)
			{
				std::cout << "Waiting for connection..." << std::endl; 
				continue;
			}
			for (nfds_t j = 0; j < pfds.size(); ++j)
			{
				if (!(pfds[j].revents & POLLIN) && !(pfds[j].revents & POLLOUT))
					continue;
				std::cout << "Ready for I/O operation" << '\n';
				if (pfds[j].fd == server_fd && (pfds[j].revents & POLLIN))
					add_client_to_pollfds(pfds, server_fd, j, client_list);
				else if (pfds[j].fd != server_fd && (pfds[j].revents & POLLIN))
					client_list[j].handle_request(pfds[j]);
				if (pfds[j].fd != server_fd && (pfds[j].revents & POLLOUT))
					client_list[j].send_answer(pfds[j]);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	close(server_fd);
	return (0);
}
