#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

#define PORT "8081"
#define IP "127.0.0.1"

int main()
{
	//1 - creation socket;
	struct addrinfo hint;
	struct addrinfo *addr;
	//struct addrinfo *res;
	std::memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_flags = AI_PASSIVE;
	hint.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(IP, PORT, &hint, &addr) != 0)
		return (std::cerr << "addrinfo" << std::endl, 1);
	int socket_fd = socket(addr->ai_family, addr->ai_socktype, 0);
	if (socket_fd < 0)
		return (std::cerr << "socket creation" << std::endl, 1);
	//2 - bind socket
	//std::cout << "socket_fd= " << socket_fd << ", ai_addr= " << addr->ai_addr->sa_data << ", ai_addrlen= " << addr->ai_addrlen << std::endl;
	if (bind(socket_fd, addr->ai_addr, addr->ai_addrlen) != 0)
		return (std::cerr << "bind socket" << std::endl, 1);
	//3 - listen && accept
	if (listen(socket_fd, 1001) < 0)
		return (std::cerr << "listen" << std::endl, 1);
	int new_socket = accept(socket_fd, NULL, 0);
	if (new_socket < 0)
		return (std::cerr << "accept" << std::endl, 1);
	freeaddrinfo(addr);
	close(new_socket);
	close(socket_fd);
	return (0);
}