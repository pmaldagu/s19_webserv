#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "lib.hpp"

class Webserv
{
	public:
		Webserv();
		Webserv(Webserv const& copy);
		~Webserv();

		Webserv& operator=(Webserv const& copy);

		/*Socket management*/
		void createSocket( void );
		void setNonblocking( void );
		void bindSocket( void );
		void setListen( void );
		void initializePoll( void );
		void launch( void );

	private:
		std::list<class Server> _servers;
		struct pollfd* _fds;

};

#endif