#include "../include/config.hpp"
#include "../include/server.hpp"
#include "../include/location.hpp"

/***********************************************************/
/***********************************************************/
/*******************    Config Class    ********************/
/***********************************************************/
/***********************************************************/

Config::Config() {}

Config::Config(std::string path) : _serv_vector()
{
    std::ifstream   ifs(path);
    std::string     line;

    if (!ifs.is_open())
        throw std::exception();
    while (getline(ifs, line))
        this->_data.push_back(line);

    for (int a = 0; a < _data.size(); a++)
        std::cout << _data[a] << std::endl;
}

Config::Config(Config const& copy)
{
    *this = copy;
}

Config::~Config() {}

Config& Config::operator=(Config const& copy)
{
    if (this != &copy)
    {
        this->_data.clear();
        for (int a = 0; a < copy._data.size(); a++)
            this->_data.push_back(copy._data[a]);
        this->_serv_vector.clear();
        for (int a = 0; a < copy._serv_vector.size(); a++)
            this->_serv_vector.push_back(copy._serv_vector[a]);
    }
    return (*this);
}

void Config::setServ()
{
    std::vector<std::string>::iterator  it;
    std::vector<std::string>::iterator  it_end = this->_data.end();
    size_t                              a = 0;

    for (it = this->_data.begin(); it != it_end; it++)
    {
        if ((*it).find("server {") != std::string::npos)
        {
            Server *newServ = new Server;
            this->_serv_vector.push_back(*newServ);
            while ((*it).find("}") == std::string::npos)
            {
                if ((*it).find("root") != std::string::npos)
                    this->_serv_vector[a].setRoot(*it);
                else if ((*it).find("listen") != std::string::npos)
                    this->_serv_vector[a].setPort(*it);
                else if ((*it).find("host") != std::string::npos)
                    this->_serv_vector[a].setHost(*it);
                else if ((*it).find("timeout") != std::string::npos)
                    this->_serv_vector[a].setTimeout(*it);
                it++;
            }
            a++;
        }
    }

/*
    std::vector<std::string>::iterator endBlock = _data.end();
    std::vector<std::string>::iterator found;
    int i = 0;
    
    while (index != _data.end())
    {
        endBlock = find(index, endBlock, std::string("}"));
        std::cout << *endBlock << std::endl;
        if ((found = find(index, endBlock, std::string("root"))) != endBlock)
            _serv_vector[i].setRoot(*found);
        else if ((found = find(index, endBlock, std::string("listen"))) != endBlock)
            _serv_vector[i].setPort(*found);
        else if ((found = find(index, endBlock, std::string("host"))) != endBlock)
            _serv_vector[i].setHost(*found);
        else if ((found = find(index, endBlock, std::string("timeout"))) != endBlock)
            _serv_vector[i].setTimeout(*found);
        index = endBlock + 1;
        i++;
    }
*/
}

std::vector<class  Server> Config::getServ() const
{
    return (this->_serv_vector);
}

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
        this->_root = copy._root;
        this->_port = copy._port;
        this->_host = copy._host;
        this->_timeout = copy._timeout;
    }
    return (*this);
}

void Server::setRoot(std::string myroot)
{
    myroot.erase(std::remove_if(myroot.begin(), myroot.end(), isspace), myroot.end());
    myroot.erase(0, 4);
    myroot.pop_back();
    this->_root = myroot;
}

void Server::setPort(std::string myport)
{
    myport.erase(std::remove_if(myport.begin(), myport.end(), isspace), myport.end());
    myport.erase(0, 6);
    myport.pop_back();
    this->_port = myport; // deja dans _address, changer
}

void Server::setHost(std::string myhost)
{
    myhost.erase(std::remove_if(myhost.begin(), myhost.end(), isspace), myhost.end());
    myhost.erase(0, 4);
    myhost.pop_back();
    this->_host = myhost; // deja dans _address, changer
}

void Server::setTimeout(std::string mytimeout)
{
    mytimeout.erase(std::remove_if(mytimeout.begin(), mytimeout.end(), isspace), mytimeout.end());
    mytimeout.erase(0, 7);
    mytimeout.pop_back();
    this->_timeout = mytimeout;
}

std::vector<class Location> Server::setLocation()
{

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