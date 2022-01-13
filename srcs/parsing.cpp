#include "../include/parsing.hpp"

/***********************************************************/
/***********************************************************/
/*******************    Server Class    ********************/
/***********************************************************/
/***********************************************************/

Server::Server()
{
    this->_port = "";
    this->_host = "";
    this->_timeout = "";

    //_root
    //_client_body_size
    //_autoindex
    //_index
    //_default_error_page
    //http_methods

    // memset((char *)&_address, 0, sizeof(_address));
    // _address.sin_family = AF_INET;                       //toujours AF_INET
    // _address.sin_addr.s_addr = htonl(this->_host);       //IP address
    // _address.sin_port = htons(this->_port);              //port
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
    this->_port = myport; // deja dans _address, changer
}

void Server::setHost(std::string myhost)
{
    this->_host = myhost; // deja dans _address, changer
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

/*
void parsingServerData(Server &server)
{
}

std::string getStrserv(std::string confFile)
{
    str::string str_serv;
    std::string target = "server{";
    size_t      pos = 0;


    while ((pos = confFile.find(target, pos)) != std::string::npos)
    {
        pos += target.length();
    }
    return (str_serv);
}
*/

std::list<class Server> getServerBlocks(std::string confFile)
{
    std::list<class Server> serv_list;
    std::string             target = "server{";
    size_t                  nbr_of_serv = 0;
    size_t                  pos = 0;
    size_t                  a = 0;

    while ((pos = confFile.find(target, pos)) != std::string::npos)
    {
        pos += target.length();
        nbr_of_serv++;
    }
    while (a < nbr_of_serv)
    {
        Server *server = new Server;
        //server->_str_serv = getStrserv(confFile);
        //parsingServerData(*server);
        serv_list.push_back(*server);
        a++;
    }

    //std::cout << nbr_of_serv << std::endl;
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
        std::cerr << RED << "Error while opening the configuration file" << RESET << std::endl;
        exit(1);
    }
    while (getline(ifs, line))
        strConf.append(line);
    strConf.erase(std::remove_if(strConf.begin(), strConf.end(), isspace), strConf.end());
    serv_list = getServerBlocks(strConf);
    std::cout << strConf << std::endl;
    return (serv_list);
}