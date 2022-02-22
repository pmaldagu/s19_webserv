#ifndef SERVER_HPP
# define SERVER_HPP

# include "lib.hpp"

class Server
{
    public :
        Server();
        Server(Server const& copy);
        virtual ~Server();

        Server&                             operator=(Server const& copy);

        bool                                checkLocationLine(std::string line);
        bool                                checkCGILine(std::string line);

				/*setter*/
				void																setFd(int fd);
        void                                setRoot(std::string myroot);
        void                                setPort(std::string myport);
        void                                setHost(std::string myhost);
        void                                setTimeout(std::string mytimeout);
        void                                setCmaxsize(std::string myclientbodysize);
        void                                setServername(std::string servername);
        void                                setRedirection(std::string redirection);
        void                                setIndex(std::string index);
        void                                setGetMethod(bool b);
        void                                setPostMethod(bool b);
        void                                setDeleteMethod(bool b);
        std::vector<std::string>::iterator  setCGI(std::vector<std::string>::iterator iterator);
        std::vector<std::string>::iterator  setLocation(std::vector<std::string>::iterator iterator);
        void                                setSockaddr();

				/*getter*/
				int																	getFd() const;
        std::string                         getRoot() const;
        std::string                         getPort() const;
        std::string                         getHost() const;
        std::string                         getTimeout() const;
        std::string                         getCmaxsize() const;
        std::string                         getServername() const;
        std::string                         getRedirection() const;
        std::string                         getIndex() const;
        bool                                getGetMethod() const;
        bool                                getPostMethod() const;
        bool                                getDeleteMethod() const;
        std::vector<class CGI>&             getCGI();
        std::vector<class Location>&        getLocation();
        struct sockaddr_in&                 getSockaddr();

				/*debug*/
        void                                debug();

    private:
				int																	_fd;
        std::string                         _index;
        std::string                         _root;
        std::string                         _port;
        std::string                         _host;
        std::string                         _timeout;
        std::string                         _client_max_body_size;
        std::string                         _server_name;
        std::string                         _redirection;
        bool                                _get_method;
        bool                                _post_method;
        bool                                _delete_method;
        std::vector<class Location>         _location_vector;
        std::vector<class CGI>              _cgi;
        struct sockaddr_in                  _address;
};

#endif
