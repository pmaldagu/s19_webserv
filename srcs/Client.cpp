#include "../include/lib.hpp"

Client::Client()
{
	throw std::runtime_error("invalid Client constructor");
}

Client::Client(int fd, int listen) : _ready(false), _listen(listen), _fd(fd)
{
	this->_req = 0;
}

Client::Client(Client const& copy)
{
	*this = copy;
}

Client::~Client()
{
	if (_ready)
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

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Setters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Client::setRequest(std::string buffer, Server& srv)
{
	this->_req = new Request(buffer, srv, this->_fd);
	this->_ready = true;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Getters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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
