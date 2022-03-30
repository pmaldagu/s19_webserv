#include "../include/lib.hpp"

Server::Server()
{
    this->_root = "";
    this->_port = "";
    this->_host = "127.0.0.1";
    this->_client_max_body_size = 0;
    this->_server_name = "";
    this->_error_page = "";
    this->_get_method = false;
    this->_post_method = false;
    this->_delete_method = false;

	memset(&_address, 0, sizeof(_address));
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
        this->_client_max_body_size = copy._client_max_body_size;
        this->_server_name = copy._server_name;
        this->_error_page = copy._error_page;
        this->_get_method = copy._get_method;
        this->_post_method = copy._post_method;
        this->_delete_method = copy._delete_method;
        this->_cgi.clear();
        for (size_t a = 0; a < copy._cgi.size(); a++)
            this->_cgi.push_back(copy._cgi[a]);
        this->_location_vector.clear();
        for (size_t a = 0; a < copy._location_vector.size(); a++)
            this->_location_vector.push_back(copy._location_vector[a]);
        memset(&_address, 0, sizeof(_address));
        this->_address.sin_addr.s_addr = copy._address.sin_addr.s_addr;
        this->_address.sin_port = copy._address.sin_port;
        this->_address.sin_family = copy._address.sin_family;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////// Checking ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Server::checkCGILine(std::string line)
{
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    if (!line.empty())
        return (false);
    return (true);
}

bool Server::checkLocationLine(std::string line)
{
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    if (!line.empty())
        return (false);
    return (true);
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Setters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Server::setFd(int fd)
{
	this->_fd = fd;
}

void Server::setRoot(std::string myroot)
{
    myroot.erase(std::remove_if(myroot.begin(), myroot.end(), isspace), myroot.end());
    myroot.erase(0, 4);
    this->_root = myroot;
}

void Server::setPort(std::string myport, class Server *srv)
{
    myport.erase(std::remove_if(myport.begin(), myport.end(), isspace), myport.end());
    myport.erase(0, 6);
    if (myport.size() != 4)
    {
        delete srv;
        throw std::runtime_error("Error : The port is not valid.");
    }
    this->_port = myport;
}

void Server::setHost(std::string myhost)
{
    myhost.erase(std::remove_if(myhost.begin(), myhost.end(), isspace), myhost.end());
    myhost.erase(0, 4);
    this->_host = myhost;
}

void Server::setCmaxsize(std::string myclientbodysize, class Server *srv)
{
    std::stringstream    ss;
    size_t              num;
    size_t              i = 0;

    myclientbodysize.erase(std::remove_if(myclientbodysize.begin(), myclientbodysize.end(), isspace), myclientbodysize.end());
    myclientbodysize.erase(0, 20);

    for (; isdigit(myclientbodysize[i]) != 0; i++)
        ss << myclientbodysize[i];
    ss >> num;
    if (myclientbodysize[i] == 'K')
        num *= 1000;
    else if (myclientbodysize[i] == 'M')
        num *= 1000000;
    else if (myclientbodysize[i] == 'G')
        num *= 1000000000;
    else if (myclientbodysize[i] == '\n')
    {
        delete srv;
        throw std::runtime_error("Invalid max client body size.");
    }
    this->_client_max_body_size = num;
}

void Server::setServername(std::string servername)
{
    servername.erase(std::remove_if(servername.begin(), servername.end(), isspace), servername.end());
    servername.erase(0, 11);
    this->_server_name = servername;
}

void Server::setErrorPage(std::string errorpage)
{
    errorpage.erase(std::remove_if(errorpage.begin(), errorpage.end(), isspace), errorpage.end());
    errorpage.erase(0, 10);
    this->_error_page = errorpage;
}

void Server::setGetMethod(bool b)
{
    if (b == true)
        this->_get_method = true;
}

void Server::setPostMethod(bool b)
{
    if (b == true)
        this->_post_method = true;
}

void Server::setDeleteMethod(bool b)
{
    if (b == true)
        this->_delete_method = true;
}

void Server::setSockaddr()
{
	this->_address.sin_family = AF_INET;
	if (this->_host.empty())
		this->_address.sin_addr.s_addr = INADDR_ANY;
	else
		this->_address.sin_addr.s_addr = inet_addr(this->_host.c_str());
	this->_address.sin_port = htons(atoi(this->_port.c_str()));
}

std::vector<std::string>::iterator Server::setCGI(std::vector<std::string>::iterator iterator)
{
    std::vector<std::string>::iterator it = iterator;

    CGI *newCGI = new CGI;
    while ((*it).find("}") == std::string::npos)
    {
        if ((*it).find("*") != std::string::npos)
            newCGI->setExtension(*it);
        else if ((*it).find("root") != std::string::npos)
            newCGI->setRoot(*it);
        else if (!checkCGILine(*it))
        {
            delete newCGI;
            throw std::runtime_error("(.conf parsing CGI): line parsing failed.");
        }
        it++;
    }
    this->_cgi.push_back(*newCGI);
    delete newCGI;
    return (it);
}
	
std::vector<std::string>::iterator Server::setLocation(std::vector<std::string>::iterator iterator)
{
    std::vector<std::string>::iterator it = iterator;

    Location *newLocation = new Location;
    while ((*it).find("}") == std::string::npos)
    {
        if ((*it).find("location") != std::string::npos)
            newLocation->setPath(*it, newLocation);
        else if ((*it).find("root") != std::string::npos)
            newLocation->setRoot(*it);
        else if ((*it).find("autoindex") != std::string::npos)
            newLocation->setAutoIndex(*it);
        else if ((*it).find("index") != std::string::npos)
            newLocation->setIndex(*it);
        else if ((*it).find("upload_dir") != std::string::npos)
            newLocation->setUploadDir(*it);
        else if ((*it).find("return") != std::string::npos)
            newLocation->setRedirection(*it);
        else if ((*it).find("http_methods") != std::string::npos)
        {
            if ((*it).find("GET") != std::string::npos)
                newLocation->setGetMethod(true);
            if ((*it).find("POST") != std::string::npos)
                newLocation->setPostMethod(true);
            if ((*it).find("DELETE") != std::string::npos)
                newLocation->setDeleteMethod(true);
        }
        else if (!checkLocationLine(*it))
        {
            delete newLocation;
            throw std::runtime_error("(.conf parsing Location): line parsing failed.");
        }
        it++;
    }
    if (newLocation->getGetMethod() == 0 && newLocation->getPostMethod() == 0 && newLocation->getDeleteMethod() == 0)
    {
       newLocation->setGetMethod(this->getGetMethod());
       newLocation->setPostMethod(this->getPostMethod());
       newLocation->setDeleteMethod(this->getDeleteMethod());
    }
    if (!newLocation->getIndex().empty())
        newLocation->setAutoIndex("autoindex off");
    this->_location_vector.push_back(*newLocation);
    delete newLocation;
    return (it);
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Guetters //////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Server::getFd() const
{
		return (this->_fd);
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

size_t Server::getCmaxsize() const
{
    return (this->_client_max_body_size);
}

std::vector<class Location>& Server::getLocation()
{
    return (this->_location_vector);
}

std::string Server::getServername() const
{
    return (this->_server_name);
}

std::string Server::getErrorPage() const
{
    return (this->_error_page);
}

bool Server::getGetMethod() const
{
    return (this->_get_method);
}

bool Server::getPostMethod() const
{
    return (this->_post_method);
}

bool Server::getDeleteMethod() const
{
    return (this->_delete_method);
}

std::vector<class CGI>& Server::getCGI()
{
    return (this->_cgi);
}

struct sockaddr_in& Server::getSockaddr()
{
    return (this->_address);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////// debug ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Server::debug()
{
    ////////////// Print Server /////////////////
    std::cout << GREEN << "-> Server " << RESET << this << std::endl;
    std::cout << GREEN << "   -Root : " << RESET << this->getRoot() << std::endl;
    std::cout << GREEN << "   -Port : " << RESET << this->getPort() << std::endl;
    std::cout << GREEN << "   -Host : " << RESET << this->getHost() << std::endl;
    std::cout << GREEN << "   -Error page : " << RESET << this->getErrorPage() << std::endl;
    std::cout << GREEN << "   -Size : " << RESET << this->getCmaxsize() << std::endl;
    std::cout << GREEN << "   -Server name : " << RESET << this->getServername() << std::endl;
    std::cout << GREEN << "   -Get method : " << RESET << this->getGetMethod() << std::endl;
    std::cout << GREEN << "   -Post method : " << RESET << this->getPostMethod() << std::endl;
    std::cout << GREEN << "   -Delete method : " << RESET << this->getDeleteMethod() << std::endl;

    ////////////// Print Location //////////////
    for (size_t a = 0; a < this->getLocation().size(); a++)
    {
        std::cout << YELLOW << "   -> Location " << RESET << &this->getLocation()[a] << std::endl;
        std::cout << YELLOW << "      -Path : " << RESET << this->getLocation()[a].getPath() << std::endl;
        std::cout << YELLOW << "      -Root : " << RESET << this->getLocation()[a].getRoot() << std::endl;
        std::cout << YELLOW << "      -Index : " << RESET << this->getLocation()[a].getIndex() << std::endl;
        std::cout << YELLOW << "      -Upload Dir : " << RESET << this->getLocation()[a].getUploadDir() << std::endl;
        std::cout << YELLOW << "      -Auto Index : " << RESET << this->getLocation()[a].getAutoIndex() << std::endl;
        std::cout << YELLOW << "      -Return : " << RESET << this->getLocation()[a].getRedirection() << std::endl;
        std::cout << YELLOW << "      -Get method : " << RESET << this->getLocation()[a].getGetMethod() << std::endl;
        std::cout << YELLOW << "      -Post method : " << RESET << this->getLocation()[a].getPostMethod() << std::endl;
        std::cout << YELLOW << "      -Delete method : " << RESET << this->getLocation()[a].getDeleteMethod() << std::endl;
    }
}
