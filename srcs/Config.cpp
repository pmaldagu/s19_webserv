#include "../include/lib.hpp"

Config::Config()
{

}

Config::Config(char* path) : _serv_vector()
{
    std::ifstream                       ifs(path);
    std::string                         line;
    size_t                              a = 0;

    if (!ifs.is_open())
        throw std::runtime_error("is_open() failed");
    while (getline(ifs, line))
        this->_data.push_back(line);

    std::vector<std::string>::iterator  it;
    std::vector<std::string>::iterator  it_end = this->_data.end();
    for (it = this->_data.begin(); it != it_end; it++)
    {
        if ((a = (*it).find(";")) != std::string::npos)
            (*it).erase(a, (*it).length() - a);
        if ((a = (*it).find("#")) != std::string::npos)
            (*it).erase(a, (*it).length() - a);
    }
    ifs.close();
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
        for (size_t a = 0; a < copy._data.size(); a++)
            this->_data.push_back(copy._data[a]);
        this->_serv_vector.clear();
        for (size_t a = 0; a < copy._serv_vector.size(); a++)
            this->_serv_vector.push_back(copy._serv_vector[a]);
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////// Checking ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
bool Config::checkServerLine(std::string line)
{
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    if (line.find("server") != std::string::npos)
    {
        if ((line.find("server{") == std::string::npos) || (line[line.size() - 1] != '{'))
            return (false);
        return (true);
    }
    if (!line.empty())
        return (false);
    return (true);
}

void Config::checkPort()
{
    size_t b = 0;

    for (size_t a = 0; a < this->_serv_vector.size(); a++)
    {
        b = a + 1;
        for (; b < this->_serv_vector.size(); b++)
        {
            if (this->_serv_vector[a].getPort() == this->_serv_vector[b].getPort() && this->_serv_vector[a].getHost() == this->_serv_vector[b].getHost())
            {
                //if ((this->_serv_vector[a].getServername().empty() || this->_serv_vector[b].getServername().empty()) || (this->_serv_vector[a].getServername() == this->_serv_vector[b].getServername()))
                    throw std::runtime_error("(.conf parsing Config): Several ports have the same name");
            }
        }
    }
}

void Config::checkPathLocation()
{
    std::vector<class Server>::iterator it_serv = _serv_vector.begin();
    bool                                slashpath = false;

    for (; it_serv != _serv_vector.end(); it_serv++)
    {
        std::vector<class Location>::iterator  it = (*it_serv).getLocation().begin();
        size_t b = 0;
        for (; it != (*it_serv).getLocation().end(); it++)
        {
            if (it->getPath() == "/")
                slashpath = true;
        }
        if (!slashpath)
            throw std::runtime_error("(.conf parsing Server): No '/' path in location");
        for (size_t a = 0; a < (*it_serv).getLocation().size(); a++)
        {
            b = a + 1;
            for (; b < (*it_serv).getLocation().size(); b++)
            {
                if ((*it_serv).getLocation()[a].getPath() == (*it_serv).getLocation()[b].getPath())
                    throw std::runtime_error("(.conf parsing Server): Several paths have the same name");
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////// Setters //////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Config::setServ()
{
    std::vector<std::string>::iterator  it;
    std::vector<std::string>::iterator  it_end = this->_data.end();

    for (it = this->_data.begin(); it != it_end; it++)
    {
        if ((*it).find("server") != std::string::npos)
        {
            Server *newServ = new Server;
            while ((*it).find("}") == std::string::npos)
            {
                if ((*it).find("location") != std::string::npos)
                {
                    if ((*it).find("*") != std::string::npos)
                        it = newServ->setCGI(it);
                    else
                        it = newServ->setLocation(it);
                }
                else if ((*it).find("root") != std::string::npos)
                    newServ->setRoot(*it);
                else if ((*it).find("listen") != std::string::npos)
                    newServ->setPort(*it, newServ);
                else if ((*it).find("host") != std::string::npos)
                    newServ->setHost(*it);
                else if ((*it).find("client_max_body_size") != std::string::npos)
                    newServ->setCmaxsize(*it, newServ);
                else if ((*it).find("error_page") != std::string::npos)
                    newServ->setErrorPage(*it);
                else if ((*it).find("server_name") != std::string::npos)
                    newServ->setServername(*it);
                else if ((*it).find("http_methods") != std::string::npos)
                {
                    if ((*it).find("GET") != std::string::npos)
                        newServ->setGetMethod(true);
                    if ((*it).find("POST") != std::string::npos)
                        newServ->setPostMethod(true);
                    if ((*it).find("DELETE") != std::string::npos)
                        newServ->setDeleteMethod(true);
                } 
                else if (!checkServerLine(*it))
                {
                    delete newServ;
                    throw std::runtime_error("(.conf parsing Server): line parsing failed.");
                }
                it++;
            }
            if (newServ->getGetMethod() == 0 && newServ->getPostMethod() == 0 && newServ->getDeleteMethod() == 0)
            {
                newServ->setGetMethod(true);
                newServ->setPostMethod(true);
                newServ->setDeleteMethod(true);
            }
			newServ->setSockaddr();
            this->_serv_vector.push_back(*newServ);
            delete newServ;
        }
    }
    checkPathLocation();
    checkPort();
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Getters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::vector<class  Server>& Config::getServ()
{
    return (this->_serv_vector);
}