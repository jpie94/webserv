#include "Server.hpp"

void	Server::add_client_to_pollfds()//server class
{
	int socket_fd;
	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		Webserv::throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	this->_pfds.push_back(temp);
	Client temp1(socket_fd, (this->_pfds.size() -1));
	this->_socket_list.push_back(temp1);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
}

void	Server::erase_from_pollfd(nfds_t &j)//server class
{
	if (close(this->_pfds[j].fd) < 0)
		throw_error("close");
	this->_pfds.erase(this->_pfds.begin() + j);
	this->_socket_list.erase(this->_socket_list.begin() + j);
	for(unsigned int i = j; i < this->_socket_list.size(); ++i)
		this->_socket_list[i]._index--;
	//Faudra penser a mettre a jour l attribut index dans tous les clients en fonction de cet erase pour faire correspondre a la structure de pollfds
}

int	Server::getType() const
{
	return (this->_type);
}