#include "../include/lib.hpp"

/***********************************************************/
/***********************************************************/
/*******************    Config Class    ********************/
/***********************************************************/
/***********************************************************/

Config::Config() {}

Config::Config(std::string path) : _serv_vector()
{
    std::ifstream                       ifs(path);
    std::string                         line;
    size_t                              a = 0;

    if (!ifs.is_open())
        throw std::exception();
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

    //////////// print .conf //////////////
    for (int a = 0; a < _data.size(); a++)
        std::cout << _data[a] << std::endl;
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

    for (it = this->_data.begin(); it != it_end; it++)
    {
        if ((*it).find("server {") != std::string::npos)
        {
            Server *newServ = new Server;
            while ((*it).find("}") == std::string::npos)
            {
                //if ((*it).find("location {") != std::string::npos)
                    //it = newServ.setLocation();
                if ((*it).find("root") != std::string::npos)
                    newServ->setRoot(*it);
                else if ((*it).find("listen") != std::string::npos)
                    newServ->setPort(*it);
                else if ((*it).find("host") != std::string::npos)
                    newServ->setHost(*it);
                else if ((*it).find("timeout") != std::string::npos)
                    newServ->setTimeout(*it);
                it++;
            }
			//newServ.setLocation();
			newServ->setSockaddr();
            //std::cout << "newServ : " << &newServ->getSockaddr() << std::endl;
            this->_serv_vector.push_back(*newServ);
            //std::cout << "_serv_vector : " << &_serv_vector[0].getSockaddr() << std::endl;
        }
    }
}

std::vector<class  Server> Config::getServ() const
{
    return (this->_serv_vector);
}
