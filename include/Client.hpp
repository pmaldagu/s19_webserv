#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "lib.hpp"

class Client
{
    public :
			Client();
			Client(int fd, int listen);
			Client(Client const& copy);
			virtual ~Client();

			Client&			operator=(Client const& copy);

			/*setter*/
			void			setRequest(std::string buffer, Server& srv);

			/*getter*/
			int				getListen( void ) const;
			int				getFd( void ) const;
			class Request&	getRequest( void ) const;
			bool 			isReady( void ) const;

    private :
			bool			_ready;
			int				_listen;
			int				_fd;
			Request*		_req;
};

#endif
