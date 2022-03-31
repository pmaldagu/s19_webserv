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

		Webserv& 							operator=(Webserv const& copy);

		/*getter*/
		class Server&						getServer(int fd);

		/*Socket management*/
		void								createSocket( void );
		void								setNonblocking( void );
		void								bindSocket( void );
		void								setListen( void );
		void								initializePoll( void );
		
		/*Multiplexer*/
		void								launch( void );
		int									setFds( void );
		void								acceptConnection(std::list<class Server>::iterator it, std::string set);
		char*								chunckMaster(char *ptr);
		std::list<class Client>::iterator	receiveRequest(std::list<class Client>::iterator it);
		std::list<class Client>::iterator	sendResponse(std::list<class Client>::iterator it);

	private:
		std::list<class Server> 			_servers;
		std::list<class Client> 			_clients;
		fd_set								readfds;
		fd_set								writefds;

};

#endif
