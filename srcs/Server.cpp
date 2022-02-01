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
    this->_client_max_body_size = "";

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

Server::~Server() {}

Server& Server::operator=(Server const& copy)
{
    if (this != &copy)
    {
        this->_root = copy._root;
        this->_port = copy._port;
        this->_host = copy._host;
        this->_timeout = copy._timeout;
        this->_client_max_body_size = copy._client_max_body_size;
        this->_location_vector.clear();
        for (int a = 0; a < copy._location_vector.size(); a++)
            this->_location_vector.push_back(copy._location_vector[a]);
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
    //std::cout << this->_root << std::endl;
}

void Server::setPort(std::string myport)
{
    myport.erase(std::remove_if(myport.begin(), myport.end(), isspace), myport.end());
    myport.erase(0, 6);
    this->_port = myport;
    //std::cout << this->_port << std::endl;
}

void Server::setHost(std::string myhost)
{
    myhost.erase(std::remove_if(myhost.begin(), myhost.end(), isspace), myhost.end());
    myhost.erase(0, 4);
    this->_host = myhost;
}

void Server::setTimeout(std::string mytimeout)
{
    mytimeout.erase(std::remove_if(mytimeout.begin(), mytimeout.end(), isspace), mytimeout.end());
    mytimeout.erase(0, 7);
    this->_timeout = mytimeout;
}

void Server::setCmaxsize(std::string& myclientbodysize)
{
    myclientbodysize.erase(std::remove_if(myclientbodysize.begin(), myclientbodysize.end(), isspace), myclientbodysize.end());
    myclientbodysize.erase(0, 20);
    this->_client_max_body_size = myclientbodysize;
}

void Server::setSockaddr()
{
	this->_address.sin_family = AF_INET;
	if (this->_host.empty())
		this->_address.sin_addr.s_addr = INADDR_ANY;
	else
		this->_address.sin_addr.s_addr = inet_addr(this->_host.c_str());
	this->_address.sin_port = htons(std::stoul(this->_port, nullptr, 0));
}
	
std::vector<std::string>::iterator Server::setLocation(std::vector<std::string>::iterator iterator, std::vector<std::string>::iterator iterator2)
{
    std::vector<std::string>::iterator it = iterator;
    std::vector<std::string>::iterator it_end = iterator2;

    Location *newLocation = new Location;
    while ((*it).find("}") == std::string::npos)
    {
        if ((*it).find("location") != std::string::npos)
            newLocation->setPostlocation(*it);
        else if ((*it).find("index") != std::string::npos)
            newLocation->setIndex(*it);
        it++;
    }
    this->_location_vector.push_back(*newLocation);
    delete newLocation;
    return (it);
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

std::string Server::getCmaxsize() const
{
    return (this->_client_max_body_size);
}

std::vector<class Location> Server::getLocation() const
{
    return (this->_location_vector);
}

struct sockaddr_in& Server::getSockaddr()
{
    return (this->_address);
}

void Server::debug() const
{
    ///// Print Server //////
    std::cout << GREEN << "-> Server " << RESET << this << std::endl;
    std::cout << GREEN << "   -Root : " << RESET << this->getRoot() << std::endl;
    std::cout << GREEN << "   -Port : " << RESET << this->getPort() << std::endl;
    std::cout << GREEN << "   -Host : " << RESET << this->getHost() << std::endl;
    std::cout << GREEN << "   -TimeOut : " << RESET << this->getTimeout() << std::endl;
    std::cout << GREEN << "   -Client : " << RESET << this->getCmaxsize() << std::endl;

    ///// Print Location //////
    for (int a = 0; a < this->getLocation().size(); a++)
    {
        std::cout << YELLOW << "   -> Location " << RESET << &this->getLocation()[a] << std::endl;
        std::cout << YELLOW << "       -Index : " << RESET << this->getLocation()[a].getIndex() << std::endl;
        std::cout << YELLOW << "       -Post Location : " << RESET << this->getLocation()[a].getPostlocation() << std::endl;
    }
}