#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "webserv.hpp"

class Config
{
    public :
        Config(std::string path);
        Config(Config const& copy);
        ~Config();

        Config& operator=(Config const& copy);

        void                                setServ();

        std::vector<class Server>           getServ() const;

    private :
        Config();
        std::vector<std::string>            _data;
        std::vector<class Server>           _serv_vector;
};

#endif