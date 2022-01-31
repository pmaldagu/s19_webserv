/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/31 16:51:48 by pmaldagu         ###   ########.fr       */
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
	
	std::cout << "ADDR : " << _servers[0].getSockaddr().sin_port << std::endl;
	for(int index = 0 ; index < _servers.size(); index++) 
	{
		//std::cout << "INDEX : " << index << std::endl;
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
	fd_set readfds;

	/*debug*/
	char* buffer = "HTTP/1.1 200 OK\nContent-type: text/plain\nContent-Length: 12\n\nHello world!";
	int addrlen = sizeof(struct sockaddr_in);
	while (true)
	{
		max_sd = -1;
		/*clear the socket set*/
		FD_ZERO(&readfds);

		/*add master sockets*/
		for	(int i = 0; i < _masterfds.size(); i++)
		{
			FD_SET(_masterfds[i], &readfds);
			if (_masterfds[i] > max_sd)
				max_sd = _masterfds[i];
		}
		/*add client socket*/
		for (int i = 0; i < _clientfds.size(); i++)
		{	
			if (_clientfds[i] > 0)
			{
				FD_SET(_clientfds[i], &readfds);
				if (_clientfds[i] > max_sd)
					max_sd = _clientfds[i];
			}
		}
		/*select*/
		ret = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if (ret < 0)
			throw std::runtime_error("select() failed");
		/*check master side*/
		for (int i = 0; i < _masterfds.size(); i++)
		{
			if (FD_ISSET(_masterfds[i], &readfds))
			{
				/*debug*/
				std::cout << "Il se passe quelque chose sur le port : " << ntohs(_servers[i].getSockaddr().sin_port) << std::endl;
				/*accept connect*/
				if ((new_socket = accept(_masterfds[i], (struct sockaddr *)&_servers[i].getSockaddr(), (socklen_t*)&addrlen)) < 0)
					throw std::runtime_error("accept() failed");
				/*send message*/
				if (send(new_socket, buffer, strlen(buffer), 0) != strlen(buffer))
					throw std::runtime_error("send() failed");
				_clientfds.push_back(new_socket);
			}
		}
		/*check client side
		 * A FAIRE
		 */		
	}
}
