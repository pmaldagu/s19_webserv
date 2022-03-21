#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "lib.hpp"

class Config
{
    public :
        Config(char* path);
        Config(Config const& copy);
        ~Config();

        Config&                     operator=(Config const& copy);

        /*Checking*/
        bool                        checkServerLine(std::string line);
        void                        checkPort();
        void                        checkPathLocation();

        /*Setters*/
        void                        setServ();

        /*Getters*/
        std::vector<class Server>&  getServ();

    private :
        Config();
        std::vector<std::string>    _data;
        std::vector<class Server>   _serv_vector;
};

#endif