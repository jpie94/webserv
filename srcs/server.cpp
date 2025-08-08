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
#include <exception>

#define PORT "8081"
#define IP "127.0.0.1"

int	make_listening_socket()
{
	struct addrinfo hint;
	struct addrinfo *addr;
	//struct addrinfo *res;
	std::memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_PASSIVE;
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(IP, PORT, &hint, &addr) != 0)
	{
		std::cerr << "Error: addrinfo" << std::endl;
		throw std::exception();
	}
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
	{
		freeaddrinfo(addr);
		std::cerr << "Error: socket creation" << std::endl;
		throw std::exception();
	}
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
	{
		freeaddrinfo(addr);
		std::cerr << "Error: bind socket" << std::endl;
		throw std::exception();
	}
	freeaddrinfo(addr);
	if (listen(socket_fd, 1001) < 0)
	{
		std::cerr << "Error: listen" << std::endl;
		throw std::exception();
	}
	return (socket_fd);
}

struct pollfd*	add_client_to_pollfds(struct pollfd *pfds, int socket_fd, int server_fd, nfds_t &len)
{
	socket_fd = accept(server_fd, NULL, 0); // On est sur un socket server en position POLLIN
	if (socket_fd < 0)
	{
		std::cerr << "Error: accept" << std::endl;
		throw std::exception();
	}
	struct pollfd *new_pollfd = new struct pollfd [len + 1];
	if (!new_pollfd)
	{
		std::cerr << "Error: new" << std::endl;
		throw std::exception();
	}
	for (nfds_t j = 0; j < len; ++j)
		new_pollfd[j] = pfds[j];
	delete [] pfds;
	std::memset(new_pollfd, 0, sizeof(struct pollfd *) * (len + 1));
	new_pollfd[len - 1].fd = socket_fd;
	new_pollfd[len - 1].events = POLLIN | POLLOUT;
	(len)++;
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
	return (new_pollfd);
}

struct pollfd*	send_msg_to_client(struct pollfd *pfds, nfds_t &len, nfds_t j)
{
	std::cout << "Socket client en position POLLIN : envoi d un message" << std::endl;
	const char *message = "lol\n";
	ssize_t count = 0;
	while (count < static_cast<ssize_t>(std::strlen(message)))
		count += send(pfds[j].fd, message, std::strlen(message), 0);
	if (close(pfds[j].fd) < 0)
	{
		std::cerr << "Error: close" << std::endl;
		throw std::exception();
	}
	struct pollfd *new_pollfd = new struct pollfd [len - 1];
	if (!new_pollfd)
	{
		std::cerr << "Error: new" << std::endl;
		throw std::exception();
	}
	int k = -1;
	for (nfds_t i = 0; i < len; ++i)
		if (i != j)
			new_pollfd[++k] = pfds[i];
	delete [] pfds;
	len--;
	return (new_pollfd);
}

int	main()
{
	try
	{
		nfds_t len = 1;
		struct pollfd *pfds = new struct pollfd[len];
		int socket_fd = 0;
		int status, server_fd = make_listening_socket();
		if (server_fd < 0)
			return (1);
		std::memset(pfds, 0, sizeof(struct pollfd *) * len);
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
			for (nfds_t j = 0; j < len; ++j)
			{
				if ((pfds[j].revents & POLLIN) != 1)
					continue;
				std::cout << "Ready for I/O operation" << '\n';
				if (pfds[j].fd == server_fd)
					pfds = add_client_to_pollfds(pfds, socket_fd, server_fd, len);
				else if (pfds[j].fd != server_fd) // On est sur un socket client en position POLLIN
					pfds = send_msg_to_client(pfds, len, j);
				// else if ((pfds[j].fd != server_fd) && (pfds[j].revents & POLLOUT) == 1) // On est sur un socket client en position POLLOUT
				// 	std::cout << "Socket client en position POLLIN" << std::endl;
				// else if ((pfds[j].revents & POLLOUT) == 1 && (pfds[j].fd == server_fd))
				// 	std::cout << "LE SERVEUR EN POLLOUT" << std::endl;
				// else
				// 	std::cout << "autre wtf" << '\n';
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
