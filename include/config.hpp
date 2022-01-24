#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <list>
# include <vector>
# include <string>
# include <iostream>
# include <fstream>
# include <algorithm>
# include <exception>
# include <netinet/in.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

class Config
{
    public :
        Config();
        Config(std::string path);
        Config(Config const& copy);
        ~Config();

        Config& operator=(Config const& copy);

    private :
        std::vector<std::string>    _data;

};

class Server
{
    public :
        Server();
        Server(Server const& copy);
        ~Server();

        Server&     operator=(Server const& copy);

        std::string getPort() const;
        std::string getHost() const;
        std::string getTimeout() const;
        //sockaddr_in getSockaddr_in() const;

        void        setPort(std::string myport);
        void        setHost(std::string myhost);
        void        setTimeout(std::string mytimeout);
        //void        setSockaddr_in();

    private:
        std::string                 _str_serv;
        std::string                 _port; // deja dans _address
        std::string                 _host; // deja dans _address
        std::string                 _timeout;
        std::list<class Location>   list_locations;
        struct sockaddr_in          *_address;
};

class Location : public Server
{
    public :
        Location();
        Location(Location const& copy);
        ~Location();

        Location& operator=(Location const& copy);
};

std::list<class Location>   getLocationsBlocks(std::string confFile);
std::list<class Server>     getServerBlocks(std::string confFile);
std::list<class Server>     parsingConf(std::string confFile);

#endif