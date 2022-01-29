#include "../include/lib.hpp"

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
