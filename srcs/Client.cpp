#include "../include/lib.hpp"

Client::Client()
{
	throw std::runtime_error("invalid Client constructor");
}

Client::Client(int fd, int listen) : _fd(fd), _listen(listen), _ready(false)
{
	this->_req = 0;
}

Client::Client(Client const& copy)
{
	*this = copy;
}

Client::~Client()
{
	delete this->_req;
}

Client& Client::operator=(Client const& copy)
{
	if (this != &copy)
	{
		this->_ready = copy._ready;
		this->_listen = copy._listen;
		this->_fd = copy._fd;
		if (copy._ready)
			this->_req = new Request(*(copy._req));
	}
	return (*this);
}

/*setter*/
void Client::setRequest(class Request newRequest)
{
	this->_req = new Request(newRequest);
	this->_ready = true;
}

/*getter*/
int Client::getListen( void ) const
{
	return (this->_listen);
}

int Client::getFd( void ) const
{
	return (this->_fd);
}

class Request& Client::getRequest(void) const
{
	return (*this->_req);
}

bool Client::isReady( void ) const
{
	return (this->_ready);
}
