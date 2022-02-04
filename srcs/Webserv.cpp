/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/02/04 13:58:17 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

// https://www.ibm.com/docs/en/i/7.4?topic=designs-example-nonblocking-io-select

#define READ 0
#define WRITE 1

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

int Webserv::setFds( void )
{
	int max_sd = -1;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

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
	return (max_sd);
}

void Webserv::acceptConnection(int index, int flag)
{
	int new_socket;
	int on = 1;
	int addrlen = sizeof(struct sockaddr_in);
	
	if (flag == READ)
		std::cout << YELLOW << "=> READ" << RESET << std::endl;
	else
		std::cout << YELLOW << "=> WRITE" << RESET << std::endl;

	std::cout << GREEN << "-> New connection" << RESET << std::endl;
	
	new_socket = accept(_masterfds[index], (struct sockaddr *)&_servers[index].getSockaddr(), (socklen_t*)&addrlen);
	if (new_socket < 0)
		throw std::runtime_error("accept() failed");

	std::cout << GREEN << "  -On port : " << RESET << _servers[index].getPort() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << _masterfds[index] << std::endl;
	std::cout << GREEN << "  -Ip : " << RESET << inet_ntoa(_servers[index].getSockaddr().sin_addr) << std::endl;
	std::cout << YELLOW << "  -> Client" << RESET << std::endl;
	std::cout << YELLOW << "     -Port : " << RESET << ntohs(_servers[index].getSockaddr().sin_port) << std::endl;
	std::cout << YELLOW << "     -Socket fd : " << RESET << new_socket << std::endl;
	std::cout << YELLOW << "     -Ip : " << RESET << inet_ntoa(_servers[index].getSockaddr().sin_addr) << std::endl;

//	std::cout << "SOCKET SATUT BEFORE : " << fcntl(new_socket, F_SETFL, O_NONBLOCK) << std::endl;
//	if ((ioctl(new_socket, FIONBIO, (char *)&on)) < 0)
//			throw std::runtime_error("ioctl() failed");

//	std::cout << "SOCKET SATUT AFTER : " << fcntl(new_socket, F_SETFL, O_NONBLOCK) << std::endl;
//	if ((setsockopt(new_socket, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
//			throw std::runtime_error("setsockopt() failed");

	FD_SET(new_socket, &readfds);
	FD_SET(new_socket, &writefds);
	_clientfds.push_back(new_socket); ///must create class Client with fd and header
}

void Webserv::receiveRequest(int fd, int flag)
{
	int ret = 1;
	char buffer[30001]; // change 3000 par max body size ?;
	
	memset(buffer, 0, 30001); //change 3000 par body size;

	if (flag == READ)
		std::cout << YELLOW << "=> READ" << RESET << std::endl;
	else
		std::cout << YELLOW << "=> WRITE" << RESET << std::endl;

	memset(buffer, 0, 30001); //change 3000 par body size;
	ret = recv(fd, buffer, 30000, 0);
	if(ret < 0)
		throw std::runtime_error("recv() failed");
			//break;
		
	std::cout << GREEN << "-> Receive" << RESET << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << fd << std::endl;
	std::cout << GREEN << "  -Message : " << RESET << std::endl << buffer << std::endl;
}

void Webserv::closeClientsFd( void )
{
	for (int i = 0; i < _clientfds.size(); i++)
	{
		close(_clientfds[i]);
	}
	_clientfds.clear();
}

void Webserv::launch( void )
{
	int ret = -1;
	int max_sd;
	int new_socket;
	//int ok = 0;

	/*debug*/
	std::string greets = "HTTP/1.1 200 OK\nContent-type: text/plain\nContent-Length: 12\n\nHello world!";

	while (true)
	{
		//ok = 0;
		/*set fd*/
		max_sd = setFds();

		/*select*/
		ret = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
		if (ret <= 0)
			throw std::runtime_error("select() failed");
		
		/*check master side*/
		for (int i = 0; i < _masterfds.size(); i++)
		{
			if (FD_ISSET(_masterfds[i], &readfds))
			{
				std::cout << std::endl << BLUE << "===MASTER===" << RESET << std::endl;
				/*accept connect*/
				acceptConnection(i, READ);
				//ok = 1;
			}
		}
	
		/*check client side*/
		for (int i = 0; i < _clientfds.size(); i++)
		{
			if (FD_ISSET(_clientfds[i], &writefds))
			{
				std::cout << std::endl << RED << "===CLIENT===" << RESET << std::endl;
				receiveRequest(_clientfds[i], WRITE);
			}
			if (FD_ISSET(_clientfds[i], &readfds))
			{
				std::cout << std::endl << RED << "===CLIENT===" << RESET << std::endl;
				std::cout << YELLOW << "=> READ" << RESET << std::endl;
				send(_clientfds[i], greets.c_str(), greets.size(), 0);
			}
		}
		closeClientsFd(); // faux doit verifier keep alive
		std::cout << GREEN << "------------END LOOP-----------\n" << RESET << std::endl;
	}
}
