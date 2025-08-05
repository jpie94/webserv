#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

#define PORT "8080"
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
	int socket_fd = make_listening_socket();
	if (socket_fd < 0)
		return (1);
	int client_socket = accept(socket_fd, NULL, 0);
	if (client_socket < 0)
		return (std::cerr << "Error: accept" << std::endl, 1);
	std::cout << "Connection accept for new client" << std::endl; 
	close(client_socket);
	close(socket_fd);
	return (0);
}