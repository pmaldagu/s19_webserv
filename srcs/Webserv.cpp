/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/25 16:20:01 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

// https://www.ibm.com/docs/en/i/7.4?topic=designs-example-nonblocking-io-select

#define FD_MAX 300 // a changer pour la bonne macro

Webserv::Webserv( void )
{
}

Webserv::Webserv( Webserv const& copy )
{
}

Webserv::~Webserv( void )
{
}

/*Socket management*/
void Webserv::createSocket( void )
{
	int on = 1;
	int index = 0;
	std::list<Server>::iterator bgin;
	std::list<Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();
	
	for( ; bgin != end; bgin++) 
	{
		memset(_fds[index], 0, sizeof(_fds[index]));
		if ((_fds[index].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //create socket
			throw std::runtime_error("socket() failed");
		_fds[index].events = POLLIN | POLLOUT;
		
		if ((setsosockopt(_fds[index].fd, SOL_SOCKET,  SO_REUSEADDR, // allow to be reusable
                  (char *)&on, sizeof(on))) < 0)
			throw std::runtime_error("setsockopt() failed");

		index++;
	}
}

void Webserv::setNonblocking( void )
{
	int on = 1;
	int index = 0;
	std::list<Server>::iterator bgin;
	std::list<Server>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();
	
	for( ; bgin != end; bgin++)
	{
		if ((ioctl(_fds[index].fd , FIONBIO, (char *)&on)) < 0)
			throw std::runtime_error("ioctl() failed");

		index++;
	}
}

void Webserv::bindSocket( void )
{
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = _servers.begin();
	end = _servers.end();

	for( ; bgin != end; bgin++)
	{
		if ((bind(_fds[index].fd , _fds[index].getSockaddr(), 
				sizeof(struct sockaddr_in) < 0)
			throw std::runtime_error("bind() failed");

		index++;
	}
}

void Webserv::setListen( void )
{
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = webserv.GetServers().begin();
	end = webserv.GetServers().end();

	for( ; bgin != end; bgin++)
	{
		if ((listen(_fds[index].fd , 32) < 0) //32 pending connection possible maybe too much ??
			throw std::runtime_error("listen() failed");

		index++;
	}
}

void Webserv::initializePoll( void ) /// Constructor a remplacer par ca
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
		write(2, e.what(). strlen(e.what()));
		//closeFd(webserv)
		throw std::runtime_error("Initialization failure");
	}
}

void Webserv::launch( void )
{
	int timeout;
	int ret = 0;
	int index = 0;

	timeout = (3 * 60 * 100); ////example to change

	while (true)
	{
		ret = poll(_fds, FD_MAX, timeout);
		if (rc =< 0)
			throw std::runtime_error("poll() failed");
		if (rc == 0)
			throw std::runtime_error("timeout");
		index = 0;
		while ( i < _servers.size() )
		{
			if ( _fds[index].revents & POLLIN )
				ret = accept(_fds[index].fd, NULL, NULL);
			if (ret < 0)
				std::runtime_error("accept() failed");






}

