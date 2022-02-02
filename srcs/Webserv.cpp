/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/02/01 16:47:06 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

// https://www.ibm.com/docs/en/i/7.4?topic=designs-example-nonblocking-io-select

Webserv::Webserv()
{
	throw std::runtime_error("no config file");
}

Webserv::Webserv( std::vector<class Server> const& servers ) : _servers(servers)
{
	try
	{
		createSocket();
		setNonblocking();
		bindSocket();
		setListen();
	}
	catch(std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Initialization failure");
	}

}

Webserv::Webserv( Webserv const& copy )
{
	_servers = copy._servers;
	_masterfds = copy._masterfds;
	_clientfds = copy._clientfds;
}

Webserv::~Webserv( void )
{
	for (int i = 0; i < _masterfds.size(); i++)
		close(_masterfds[i]);
	for (int i = 0; i < _clientfds.size(); i++)
		close(_clientfds[i]);
}

/*Socket management*/
void Webserv::createSocket( void )
{
	int on = 1;
	
	for(int index = 0 ; index < _servers.size(); index++) 
	{
		_masterfds.push_back(socket(AF_INET, SOCK_STREAM, 0)); //create socket
		if (_masterfds[index] < 0)
			throw std::runtime_error("socket() failed");
		if ((setsockopt(_masterfds[index], SOL_SOCKET,  SO_REUSEADDR, // allow to be reusable
                  (char *)&on, sizeof(on))) < 0)
			throw std::runtime_error("setsockopt() failed");
	}
}

void Webserv::setNonblocking( void )
{
	int on = 1;

	for(int index = 0 ; index < _servers.size(); index++) 
	{
		if ((ioctl(_masterfds[index] , FIONBIO, (char *)&on)) < 0)
			throw std::runtime_error("ioctl() failed");
	}
}

void Webserv::bindSocket( void )
{
	for(int index = 0 ; index < _servers.size(); index++) 
	{
		if ((bind(_masterfds[index], (struct sockaddr *)&_servers[index].getSockaddr(), sizeof(struct sockaddr_in))) < 0)
			throw std::runtime_error("bind() failed");
	}
}

void Webserv::setListen( void )
{
	for(int index = 0 ; index < _servers.size(); index++) 
	{
		if ((listen(_masterfds[index], 32)) < 0) //32 pending connection possible maybe too much ??
			throw std::runtime_error("listen() failed");
	}
}


void Webserv::launch( void )
{
	//Utiliser select()
	int ret = -1;
	int max_sd;
	int new_socket;
	char request[30001]; // change 3000 par max body size;
	fd_set readfds;
	fd_set writefds;

	/*debug*/
	std::string greets = "HTTP/1.1 200 OK\nContent-type: text/plain\nContent-Length: 12\n\nHello world!";
	int addrlen = sizeof(struct sockaddr_in);
	while (true)
	{
		max_sd = -1;
		/*clear the socket set and buffer*/
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		memset(request, 0, 30001); //change 3000 par body size;

		/*add master sockets*/
		for	(int i = 0; i < _masterfds.size(); i++)
		{
			FD_SET(_masterfds[i], &readfds);
			FD_SET(_masterfds[i], &writefds);
			if (_masterfds[i] > max_sd)
				max_sd = _masterfds[i];
		}
		/*add client socket*/
		for (int i = 0; i < _clientfds.size(); i++)
		{	
			if (_clientfds[i] > 0)
			{
				FD_SET(_clientfds[i], &readfds);
				FD_SET(_clientfds[i], &writefds);
				if (_clientfds[i] > max_sd)
					max_sd = _clientfds[i];
			}
		}
		/*select*/
		ret = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
		if (ret < 0)
			throw std::runtime_error("select() failed");
		
		/*check master side*/
		std::cout << BLUE << "+++++++++++MASTER SIDE+++++++++++" << RESET << std::endl;
		for (int i = 0; i < _masterfds.size(); i++)
		{
			if (FD_ISSET(_masterfds[i], &readfds))
			{
				std::cout << YELLOW << "===========READ REQUEST==========\n" << RESET << std::endl;				
				/*debug*/
				//std::cout << "Il se passe quelque chose sur le port : " << ntohs(_servers[i].getSockaddr().sin_port) << std::endl;				
				/*accept connect*/
				if ((new_socket = accept(_masterfds[i], (struct sockaddr *)&_servers[i].getSockaddr(), (socklen_t*)&addrlen)) < 0)
					throw std::runtime_error("accept() failed");				
				/*receive request*/
				std::cout << "RECEIVE" << std::endl;
				if ((ret = recv(new_socket, request, 30000, 0)) < 0)
					throw std::runtime_error("recv() failed");
				std::cout << "END RECEIVE" << std::endl;
				std::cout << request << std::endl;
				// parser la request		
				/*send message*/
				if (send(new_socket, greets.c_str(), greets.size(), 0) != greets.size())
					throw std::runtime_error("send() failed");
				_clientfds.push_back(new_socket);
				//FD_SET(_clientfds[_clientfds.size() - 1], &readfds);
			}
			if (FD_ISSET(_masterfds[i], &writefds))
			{
				std::cout << YELLOW << "========WRITE REQUEST========\n" << RESET << std::endl;
				/*debug*/
				//std::cout << "Il se passe quelque chose sur le port : " << ntohs(_servers[i].getSockaddr().sin_port) << std::endl;
				/*accept connect*/
				if ((new_socket = accept(_masterfds[i], (struct sockaddr *)&_servers[i].getSockaddr(), (socklen_t*)&addrlen)) < 0)
					throw std::runtime_error("accept() failed");
				/*receive request*/
				if ((ret = recv(new_socket, request, 30000, 0)) < 0)
					throw std::runtime_error("recv() failed");
				std::cout << request << std::endl;
				// parser la request		
				/*send message*/
				if (send(new_socket, greets.c_str(), greets.size(), 0) != greets.size())
					throw std::runtime_error("send() failed");
				_clientfds.push_back(new_socket);
				//FD_SET(_clientfds[_clientfds.size() - 1], &readfds]);
			}			
			std::cout << GREEN << "------------END MASTER-----------\n" << RESET << std::endl;
		}
		
		/*check client side*/
		memset(request, 0, 30001); //change 3000 par body size;
		std::cout << RED << "+++++++++++CLIENT SIDE+++++++++++" << RESET << std::endl;
		for (int i = 0; i < _clientfds.size(); i++)
		{
			if (FD_ISSET(_clientfds[i], &readfds))
			{
				std::cout << YELLOW << "========READ activity=======\n" << RESET << std::endl;
				if((recv(_clientfds[i], request, 30000, 0)) == 0)
				{
					std::cout << "Un client est EOF" << std::endl;
					close(_clientfds[i]);
					_clientfds[i] = 0;
					FD_CLR(_clientfds[i], &readfds);
				}
				else
					std::cout << request << std::endl;
			}
			if (FD_ISSET(_clientfds[i], &writefds))
			{
				std::cout << YELLOW << "========WRITE activity======\n" << RESET << std::endl;
				std::cout << "RECEIVE" << std::endl;
				if((recv(_clientfds[i], request, 30000, 0)) == 0)
				{
					std::cout << "Un client est EOF" << std::endl;
					close(_clientfds[i]);
					_clientfds[i] = 0;
					FD_CLR(_clientfds[i], &writefds);
				}
				else
					std::cout << request << std::endl;
				std::cout << "END RECEIVE" << std::endl;
			}
			std::cout << GREEN << "------------END CLIENT-----------\n" << RESET << std::endl;

		}
		std::cout << GREEN << "------------END LOOP-----------\n" << RESET << std::endl;
		
	}
}
