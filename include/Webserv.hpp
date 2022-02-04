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
		
		/*Multiplexer*/
		void launch( void );
		int setFds( void );
		void acceptConnection(int index, int flag);
		void receiveRequest(int fd, int flag);

		/*Close*/
		void closeClientsFd( void );

	private:
		std::vector<class Server> _servers;
		std::vector<int>  _masterfds;
		std::vector<int>  _clientfds;
		fd_set						readfds;
		fd_set						writefds;

};

#endif
