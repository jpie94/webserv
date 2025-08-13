#include "WebSocket.hpp"

// *****************_________CANONICAL______________******************
WebSocket::WebSocket(void) : _fd(), _index(), _type(), _count(), _recieved()
{
}

WebSocket::WebSocket(const WebSocket& srcs)
{
	*this = srcs;
}

WebSocket::~WebSocket(void)
{
}

WebSocket &WebSocket::operator=(WebSocket const & rhs)
{
    if (this != &rhs)
    {
	this->_fd = rhs._fd;
	this->_index = rhs._index;
	this->_type = rhs._type;
	this->_count = rhs._count;
	this->_recieved = rhs._recieved;
    }
    return (*this);
}

// *****************_________MEMBER______________******************

WebSocket::WebSocket(int i, nfds_t j, t_socket type) : _count(), _recieved()
{
	_fd = i;
	_index = j;
	this->_type = type;
}

void	WebSocket::add_client_to_pollfds()//WebSocket class
{
	int socket_fd;
	socket_fd = accept(this->_fd, NULL, 0);
	if (socket_fd < 0)
		Webserv::throw_error("accept");
	struct pollfd temp = {socket_fd, POLLIN, 0};
	this->_pfds.push_back(temp);
	WebSocket temp1(socket_fd, (this->_pfds.size() -1), CLIENT);
	this->_socket_list.push_back(temp1);
	std::cout << "Connection accepted for new client "<< socket_fd << std::endl;
}

void	WebSocket::erase_from_pollfd(nfds_t &j)//WebSocket class
{
	if (close(this->_pfds[j].fd) < 0)
		throw_error("close");
	this->_pfds.erase(this->_pfds.begin() + j);
	this->_socket_list.erase(this->_socket_list.begin() + j);
	for(unsigned int i = j; i < this->_socket_list.size(); ++i)
		this->_socket_list[i]._index--;
	//Faudra penser a mettre a jour l attribut index dans tous les clients en fonction de cet erase pour faire correspondre a la structure de pollfds
}

int	WebSocket::getType() const
{
	return (this->_type);
}


void	WebSocket::handle_request()
{
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes_read = recv(_pfds[this->_index].fd, &buffer,BUFSIZ, 0);
	// if (bytes_read < 0)
	// {
	// 	std::cout << "[" << pfds.fd << "] Error: recv, connection closed." << '\n';
	// 	erase_from_pollfd(pfds, j);
	// }
	// if (bytes_read == 0)
	// {
	// 	std::cout << "[" << pfds.fd << "] Client socket closed connection." << '\n';
	// 	erase_from_pollfd(pfds, j);
	// }
	std::cout << "index= " << _index << std::endl;
	std::cout << "byte read : " << bytes_read << std::endl;
	std::cout << "_recieved= " << _recieved << std::endl;
	_recieved = _recieved + buffer;
	_count += bytes_read;
	std::cout << "count : " << _count << std::endl;
	std::cout << "\ntaille message recu: " << _recieved.size() << '\n';
	std::cout << "[" << _pfds[_index].fd << "] Got message:\n" << _recieved << '\n';
	if (_count == HEADERLEN + BODYLEN)
	{
		_pfds[_index].events = POLLOUT;
		_count = 0;
	}
}

void	WebSocket::send_answer()
{
	const char message[] = "hello client, I want to answer you but I'm to dumb to make a reel HTTP/1.1 answer O_o\n";
	ssize_t msg_len = std::strlen(message);
	if (!msg_len)
		return((void)(std::cout << "strlen est egal a 0 pour message len" << std::endl)) ; 
	size_t sent = send(this->_pfds[this->_index].fd, message + _count, msg_len - _count, 0);
	// if (sent < 0)
	// 	throw_error("send");
	_count += sent;
	std::cout << "\ntaille message envoye: " << _count << '\n';
	if(_count == msg_len)
	{
		this->_pfds[this->_index].events = POLLIN;
		//message.erase();
		_count = 0;
	}
}