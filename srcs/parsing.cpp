#include "../include/parsing.hpp"

/***********************************************************/
/***********************************************************/
/*******************    Server Class    ********************/
/***********************************************************/
/***********************************************************/

Server::Server()
{
    this->_port = nullptr;
    this->_host = nullptr;
    this->_timeout = nullptr;
}

Server::Server(Server const& copy)
{
    *this = copy;
}

Server::~Server()
{

}

Server& Server::operator=(Server const& copy)
{
    if (this != &copy)
    {
        this->_port = copy._port;
        this->_host = copy._host;
        this->_timeout = copy._timeout;
    }
    return (*this);
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

void Server::setPort(std::string myport)
{
    this->_port = myport;
}

void Server::setHost(std::string myhost)
{
    this->_host = myhost;
}

void Server::setTimeout(std::string mytimeout)
{
    this->_timeout = mytimeout;
}

/***********************************************************/
/***********************************************************/
/*****************    Location Class   *********************/
/***********************************************************/
/***********************************************************/

Location::Location()
{

}

Location::Location(Location const & copy)
{
    *this = copy;
}

Location::~Location()
{

}

Location& Location::operator=(Location const& copy)
{
    if (this != &copy)
    {

    }
    return (*this);
}

/***********************************************************/
/***********************************************************/
/**************    Non-member functions    *****************/
/***********************************************************/
/***********************************************************/

std::list<class Location> getLocationsBlocks(std::string confFile)
{
    std::list<class Location> location_list;

    return (location_list);
}

std::list<class Server> getServerBlocks(std::string confFile)
{
    std::list<class Server> serv_list;

    return (serv_list);
}

std::list<class Server> parsingConf(std::string confFile)
{
    std::list<class Server> serv_list;
    std::ifstream           ifs(confFile);
    std::string             line;
    std::string             strConf;

    if (!ifs.is_open())
    {
        std::cout << "Error while opening the configuration file\n";
        exit(1);
    }
    while (getline(ifs, line))
        strConf.append(line);
    strConf.erase(std::remove_if(strConf.begin(), strConf.end(), isspace), strConf.end());
    serv_list = getServerBlocks(strConf);
    std::cout << strConf << std::endl;
    return (serv_list);
}