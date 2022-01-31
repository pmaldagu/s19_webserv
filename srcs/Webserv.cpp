/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/29 18:26:27 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

// https://www.ibm.com/docs/en/i/7.4?topic=designs-example-nonblocking-io-select

#define FD_MAX 300 // a changer pour la bonne macro

Webserv::Webserv()
{
}

Webserv::Webserv( std::vector<class Server> const& servers ) : _servers(servers)
{
	this->_fds = new int[servers.size()];
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
		//closeFd(webserv)
		throw std::runtime_error("Initialization failure");
	}

}

Webserv::Webserv( Webserv const& copy )
{
	delete [] this->_fds;
}

Webserv::~Webserv( void )
{
}

/*Socket management*/
void Webserv::createSocket( void )
{
	int on = 1;
	int index = 0;
	std::vector<class Server>::iterator bgin;
	std::vector<class Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();
	
	for( ; bgin != end; bgin++) 
	{
		if ((_fds[index] = socket(AF_INET, SOCK_STREAM, 0)) < 0) //create socket
			throw std::runtime_error("socket() failed");
		if ((setsockopt(_fds[index], SOL_SOCKET,  SO_REUSEADDR, // allow to be reusable
                  (char *)&on, sizeof(on))) < 0)
			throw std::runtime_error("setsockopt() failed");

		index++;
	}
}

void Webserv::setNonblocking( void )
{
	int on = 1;
	int index = 0;
	std::vector<class Server>::iterator bgin;
	std::vector<class Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();
	
	for( ; bgin != end; bgin++)
	{
		if ((ioctl(_fds[index] , FIONBIO, (char *)&on)) < 0)
			throw std::runtime_error("ioctl() failed");
		index++;
	}
}

void Webserv::bindSocket( void )
{
	int index = 0;
	std::vector<class Server>::iterator bgin;
	std::vector<class Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();

	for( ; bgin != end; bgin++)
	{
		if ((bind(_fds[index], (struct sockaddr *)&_servers[index].getSockaddr(), sizeof(struct sockaddr_in))) < 0)
			throw std::runtime_error("bind() failed");
		index++;
	}
}

void Webserv::setListen( void )
{
	int index = 0;
	std::vector<class Server>::iterator bgin;
	std::vector<class Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();

	for( ; bgin != end; bgin++)
	{
		if ((listen(_fds[index], 32)) < 0) //32 pending connection possible maybe too much ??
			throw std::runtime_error("listen() failed");

		index++;
	}
}

/*
void Webserv::launch( void )
{
	//Utiliser select()
}
*/