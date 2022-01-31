#ifndef SERVER_HPP
# define SERVER_HPP

# include "lib.hpp"

class Server
{
    public :
        Server();
        Server(Server const& copy);
        virtual ~Server();

        Server&                                 operator=(Server const& copy);

        void                                    setRoot(std::string myroot);
        void                                    setPort(std::string myport);
        void                                    setHost(std::string myhost);
        void                                    setTimeout(std::string mytimeout);
        std::vector<class Location>             setLocation();
        void                                    setSockaddr();

        std::string                             getRoot() const;
        std::string                             getPort() const;
        std::string                             getHost() const;
        std::string                             getTimeout() const;
        std::vector<class Location>             getLocation() const;    
        struct sockaddr_in&                     getSockaddr();

    private:
        std::string                             _root;
        std::string                             _port;
        std::string                             _host;
        std::string                             _timeout;
        std::vector<class Location>             _location_vector;
        struct sockaddr_in                      _address;
};

#endif
