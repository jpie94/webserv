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

#define PORT "8080"
#define IP "127.0.0.1"
#define BODYLEN 5000
#define HEADERLEN 0

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

void	add_client_to_pollfds(std::vector<struct pollfd> &pfds, int server_fd)
{
	int socket_fd;
	socket_fd = accept(server_fd, NULL, 0);
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

void	send_answer(std::vector<struct pollfd> &pfds, nfds_t &j)
{
	const char message[] = "hello client, I want to answer you but I'm to dumb to make a reel HTTP/1.1 answer O_o\n";
	ssize_t msg_len = std::strlen(message);
	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0" << std::endl)) ; 
	ssize_t count = 0;
	ssize_t	sent = 0;
	sent = send(pfds[j].fd, message + count, msg_len - count, 0);
	if (sent < 0)
		throw_error("send");
	count += sent;
	std::cout << "\ntaille message envoye: " << count << '\n';
	if(count == msg_len)
	{
		pfds[j].events = POLLIN;
		//message.erase();
		count = 0;
	}
}

void	handle_request(std::vector<struct pollfd> &pfds, nfds_t &j)
{
	int count = 0, bytes_read = 1, sender_fd = pfds[j].fd;
	char buffer[4096];
	std::string recieved;
	bytes_read = 0;
	memset(buffer, 0, sizeof(buffer));
	bytes_read = recv(sender_fd, &buffer,BUFSIZ, 0);
	if (bytes_read < 0)
	{
		std::cout << "[" << sender_fd << "] Error: recv, connection closed." << '\n';
		erase_from_pollfd(pfds, j);
	}
	if (bytes_read == 0)
	{
		std::cout << "[" << sender_fd << "] Client socket closed connection." << '\n';
		erase_from_pollfd(pfds, j);
	}
	std::cout << "byte read : " << bytes_read << std::endl;
	recieved += buffer;
	count += bytes_read;
	std::cout << "count : " << count << std::endl;
	std::cout << "\ntaille message recu: " << recieved.size() << '\n';
	std::cout << "[" << sender_fd << "] Got message:\n" << recieved << '\n';
	if (count == HEADERLEN + BODYLEN)
	{
		pfds[j].events = POLLOUT;
		count = 0;
	}
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
					add_client_to_pollfds(pfds, server_fd);
				else if (pfds[j].fd != server_fd && (pfds[j].revents & POLLIN))
					handle_request(pfds, j);
				if (pfds[j].fd != server_fd && (pfds[j].revents & POLLOUT))
					send_answer(pfds, j);
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
