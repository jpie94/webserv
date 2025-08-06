#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <vector>

#define PORT "8082"
#define IP "127.0.0.1"

int make_listening_socket()
{
	/*1 - creation socket*/
	struct addrinfo hint;
	struct addrinfo *addr;
	//struct addrinfo *res;
	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(IP, PORT, &hint, &addr) != 0)
		return (std::cerr << "Error: addrinfo" << std::endl, -1);
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
		return (std::cerr << "Error: socket creation" << std::endl, -1);
	/*2 - bind socket*/
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
		return (std::cerr << "Error: bind socket" << std::endl, -1);
	freeaddrinfo(addr);
	/*3 - listen && accept*/
	if (listen(socket_fd, 1001) < 0)
		return (std::cerr << "Error: listen" << std::endl, -1);
	return (socket_fd);
}

int main()
{
	nfds_t len = 1;
	struct pollfd *pfds = new struct pollfd[len];
	int status;
	int socket_fd;
	int server_fd = make_listening_socket();
	if (server_fd < 0)
		return (1);
	pfds[len - 1].fd = server_fd;
	pfds[len - 1].events = POLLIN;
	while(1)
	{
		status = poll(pfds, len, 2000);
		if (status < 0)
			return (std::cerr << "Error: poll" << std::endl, 1);
		if (status == 0)
		{
			std::cout << "Waiting for connection" << std::endl; 
			continue;
		}
		for (nfds_t j = 0; j < len; j++)
		{
			// if ((pfds[j].revents & POLLIN) != 1)
			// 	continue;
			// else 
			if ((pfds[j].revents & POLLIN) == 1 && (pfds[j].fd == server_fd))
			{
				socket_fd = accept(server_fd, NULL, 0); // ON est sur un socket server en position POLLIN
				{
					if (socket_fd < 0)
						return (std::cerr << "Error: accept" << std::endl, 1);
					struct pollfd *temp = new struct pollfd[len];
					for (nfds_t j = 0; j < len; j++)
						temp[j] = pfds[j];
					delete[] pfds;	
					len++;
					struct pollfd *pfds = new struct pollfd[len];
					for (nfds_t j = 0; j < len - 1; j++)
						pfds[j] = temp[j];
					delete[] temp;
					pfds[len - 1].fd = socket_fd;
					pfds[len - 1].events = POLLIN | POLLOUT;
					std::cout << "Connection accepted for new client "<< socket_fd << std::endl;

				}
			}
			else if (pfds[j].fd != server_fd && (pfds[j].revents & POLLIN) == 1) // ON est sur un socket client en position POLLIN
			{
				std::cout << "Socket client en position POLLIN : envoi d un message" << std::endl;
				const char *message = "lol\n";
				send(pfds[j].fd, message, std::strlen(message), 0);
				close(pfds[j].fd);
				struct pollfd *temp = new struct pollfd[len];
				for (nfds_t j = 0; j < len; j++)
					temp[j] = pfds[j];
				delete[] pfds;	
				len--;
				pfds = new struct pollfd[len];
				for (nfds_t j = 0; j < len; j++)
					pfds[j] = temp[j];
				delete[] temp;
			}	
			else if (pfds[j].fd != server_fd && (pfds[j].revents & POLLOUT) == 1) // ON est sur un socket client en position POLLOUT
				std::cout << "Socket client en position POLLOUT" << std::endl;
			else if ((pfds[j].revents & POLLOUT) == 1 && (pfds[j].fd == server_fd))
				std::cout << "LE SERVEUR EN POLLOUT" << std::endl;
		}
	}
	return (0);
}
