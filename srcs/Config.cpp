#include "../include/lib.hpp"

Config::Config() {}

Config::Config(char* path) : _serv_vector()
{
    std::ifstream                       ifs(path);
    std::string                         line;
    size_t                              a = 0;

    if (!ifs.is_open())
        throw std::runtime_error("is_open() failed");
    while (getline(ifs, line))
        this->_data.push_back(line);

    std::vector<std::string>::iterator  it_end = this->_data.end();
    std::vector<std::string>::iterator  it;
    for (it = this->_data.begin(); it != it_end; it++)
    {
        if ((a = (*it).find(";")) != std::string::npos)
            (*it).erase(a, (*it).length() - a);
        if ((a = (*it).find("#")) != std::string::npos)
            (*it).erase(a, (*it).length() - a);
    }
    ifs.close();

    ////////// print .conf clean //////////
    //for (int a = 0; a < _data.size(); a++)
    //     std::cout << _data[a] << std::endl;
    ///////////////////////////////////////
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

bool Config::checkServerLine(std::string line)
{
    //P(line, "line");
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
                    newServ->setPort(*it);
                else if ((*it).find("host") != std::string::npos)
                    newServ->setHost(*it);
                else if ((*it).find("timeout") != std::string::npos)
                    newServ->setTimeout(*it);
                else if ((*it).find("return") != std::string::npos)
                    newServ->setRedirection(*it);
                else if ((*it).find("client_max_body_size") != std::string::npos)
                    newServ->setCmaxsize(*it);
                else if ((*it).find("index") != std::string::npos)
                    newServ->setIndex(*it);
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
}

std::vector<class  Server>& Config::getServ()
{
    return (this->_serv_vector);
}
