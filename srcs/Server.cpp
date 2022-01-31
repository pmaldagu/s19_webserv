#include "../include/lib.hpp"

/***********************************************************/
/***********************************************************/
/*******************    Server Class    ********************/
/***********************************************************/
/***********************************************************/

Server::Server()
{
    this->_root = "";
    this->_port = "";
    this->_host = "";
    this->_timeout = "";

	//this->_address = new struct sockaddr_in;
	memset(&_address, 0, sizeof(_address));

    //_root
    //_client_body_size
    //_autoindex
    //_index
    //_default_error_page
    //http_methods
}

Server::Server(Server const& copy)
{
    *this = copy;
}

Server::~Server()
{
	//delete this->_address;
}

Server& Server::operator=(Server const& copy)
{
    if (this != &copy)
    {
        this->_root = copy._root;
        this->_port = copy._port;
        this->_host = copy._host;
        this->_timeout = copy._timeout;
		memset(&_address, 0, sizeof(_address));
		this->_address.sin_addr.s_addr = copy._address.sin_addr.s_addr;
		this->_address.sin_port = copy._address.sin_port;
		this->_address.sin_family = copy._address.sin_family;
    }
    return (*this);
}

void Server::setRoot(std::string myroot)
{
    myroot.erase(std::remove_if(myroot.begin(), myroot.end(), isspace), myroot.end());
    myroot.erase(0, 4);
    this->_root = myroot;
}

void Server::setPort(std::string myport)
{
    myport.erase(std::remove_if(myport.begin(), myport.end(), isspace), myport.end());
    myport.erase(0, 6);
    this->_port = myport;
}

void Server::setHost(std::string myhost)
{
    myhost.erase(std::remove_if(myhost.begin(), myhost.end(), isspace), myhost.end());
    myhost.erase(0, 4);
    myhost.erase(std::remove(myhost.begin(), myhost.end(), '.'), myhost.end());
    this->_host = myhost;
}

void Server::setTimeout(std::string mytimeout)
{
    mytimeout.erase(std::remove_if(mytimeout.begin(), mytimeout.end(), isspace), mytimeout.end());
    mytimeout.erase(0, 7);
    this->_timeout = mytimeout;
}

void Server::setSockaddr()
{
	// this->_address = new struct sockaddr_in;

	// memset(_address, 0, sizeof(*_address));
	this->_address.sin_family = AF_INET;
	if (this->_host.empty())
		this->_address.sin_addr.s_addr = INADDR_ANY;
	else
		this->_address.sin_addr.s_addr = htonl(std::stoul(this->_host, nullptr, 0));
	this->_address.sin_port = htons(std::stoul(this->_port, nullptr, 0));

	/*debug*/
	//std::cout << "HOST : " << std::stoul(this->_host, nullptr, 0) << std::endl;
	//std::cout << "PORTl : " << this->_address->sin_port << std::endl;
	//std::cout << "PORT : " << std::stoul(this->_port, nullptr, 0) << std::endl;
	//std::cout << "HOSTn : " << this->_address->sin_addr.s_addr << std::endl;
}
	
std::vector<class Location> Server::setLocation()
{
	return (this->_location_vector);
}

std::string Server::getRoot() const
{
    return (this->_root);
}

std::string Server::getPort() const
{
    return (this->_port);
}

std::string Server::getHost() const
{
    return (this->_host);
}

std::string Server::getTimeout() const
{
    return (this->_timeout);
}

std::vector<class Location> Server::getLocation() const
{
    return (this->_location_vector);
}

struct sockaddr_in& Server::getSockaddr()
{
    return (this->_address);
}


