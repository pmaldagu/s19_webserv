#ifndef PARSING_HPP
# define PARSING_HPP

# include <list>
# include <string>
# include <iostream>
# include <fstream>
# include <algorithm>

class Server
{
    public :
        Server();
        Server(Server const& copy);
        ~Server();

        Server& operator=(Server const& copy);

        std::string getPort() const;
        std::string getHost() const;
        std::string getTimeout() const;

        void setPort(std::string myport);
        void setHost(std::string myhost);
        void setTimeout(std::string mytimeout);

    private:
        std::string _port;
        std::string _host;
        std::string _timeout;
        std::list<class Location> list_locations;
};

class Location //: public Server
{
    public :
        Location();
        ~Location();
};

std::list<class Location> getLocationsBlocks(std::string confFile);
std::list<class Server> getServerBlocks(std::string confFile);
std::list<class Server> parsingConf(std::string confFile);

#endif