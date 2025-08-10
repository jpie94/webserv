#include <iostream>
#include <cstdio>
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
#include <sstream>
#include <csignal>

#define PORT "8081"
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
	//struct addrinfo *res;
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

void	add_client_to_pollfds(std::vector<struct pollfd> &pfds, int server_fd)
{
	int socket_fd;
	socket_fd = accept(server_fd, NULL, 0); // On est sur un socket server en position POLLIN
	if (socket_fd < 0)
		throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	pfds.push_back(temp);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
}

void	erase_from_pollfd(std::vector<struct pollfd> &pfds, nfds_t &j)
{
	if (close(pfds[j].fd) < 0)
		throw_error("close");
	pfds.erase(pfds.begin() + j--);
}

void	send_msg_to_client(int client_fd, const char *message)
{
	std::cout << "Socket client en position POLLIN : envoi d un message" << std::endl;
	ssize_t msg_len = std::strlen(message);
	ssize_t count = 0;
	ssize_t	sent = 0;
	while (count < msg_len)
	{
		sent = send(client_fd, message + count, msg_len - count, 0);
		if (sent < 0)
			throw_error("send");
		count += sent;
	}
	std::cout << "\ntaille message envoye: " << count << '\n';
	//send(client_fd,"\nOK\n", 4, 0);
}

void	handle_client_request(std::vector<struct pollfd> &pfds, nfds_t &j, std::vector<char> &buffer)
{
	int bytes_read, sender_fd;
	sender_fd = pfds[j].fd;
	char buf[BUFSIZ];
	std::memset(&buf, '\0', sizeof(buf));
	int i = -1;
	bytes_read = recv(sender_fd, &buf, BUFSIZ, 0);
	if (bytes_read < 0)
		throw_error("recv");
	if (bytes_read == 0)
	{
		std::cout << "[" << sender_fd << "] Client socket closed connection." << '\n';
		erase_from_pollfd(pfds, j);
	}
	i = -1;
	while (buf[++i])
		buffer.push_back(buf[i]);
	std::cout << "\ntaille message recu: " << buffer.size() << '\n';
	std::cout << "[" << sender_fd << "] Got message: " << buffer.data() << '\n';
	//transform cleint request into answer
	send_msg_to_client(sender_fd, buffer.data());
}

void sigIntHandler(int signal)
{
	if (signal == SIGINT)
		std::exception();	
}

int	main()
{
	try
	{
		std::signal(SIGINT, sigIntHandler);
		std::vector<struct pollfd> pfds;
		std::vector<char> buffer;
		int status, server_fd = make_listening_socket();
		if (server_fd < 0)
			return (1);
		struct pollfd temp = {server_fd, POLLIN, 0};
		pfds.push_back(temp);
		while(1)
		{
			std::memset(&buffer, '\0', sizeof(buffer));
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
					add_client_to_pollfds(pfds, server_fd);
				else if (pfds[j].fd != server_fd && (pfds[j].revents & POLLIN)) // On est sur un socket client en position POLLIN
					handle_client_request(pfds, j, buffer);
				// if (pfds[j].fd != server_fd && (pfds[j].revents & POLLOUT))
				// 	send_msg_to_client(pfds[j].fd , buffer);
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return (0);
}
