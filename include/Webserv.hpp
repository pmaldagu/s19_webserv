#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "lib.hpp"

class Webserv
{
	public:
		Webserv();
		Webserv(std::vector<class Server> const& servers);
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
		std::vector<class Server> _servers;
		int* _fds;

};

#endif
