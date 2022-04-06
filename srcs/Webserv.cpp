/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: namenega <namenega@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/03/24 18:15:04 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"
#include <cstring>

Webserv::Webserv()
{
	throw std::runtime_error("no config file");
}

Webserv::Webserv( std::vector<class Server> const& servers )
{
	
	for (size_t i = 0; i < servers.size(); i++) {
		_servers.push_back(servers[i]);
	}
	
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

Webserv::Webserv(Webserv const& copy)
{
	*this = copy;
}

Webserv& Webserv::operator=(Webserv const& copy)
{
	if (this != &copy)
	{
		this->_servers = copy._servers;
		this->_clients = copy._clients;
		this->readfds = copy.readfds;
		this->writefds = copy.writefds;
	}
	return (*this);
}
		
Webserv::~Webserv( void )
{

}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Getters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class Server& Webserv::getServer(int fd)
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		if ((*it).getFd() == fd)
			return (*it);
	}
	throw std::runtime_error("server not found");
	return (*(_servers.begin()));
}
			
///////////////////////////////////////////////////////////////////////////
///////////////////////// Socket management ///////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Webserv::createSocket( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		/*create socket*/
		(*it).setFd(socket(AF_INET, SOCK_STREAM, 0));
		if ((*it).getFd() < 0)
			throw std::runtime_error("socket() failed");

		/*set as reusable*/
		int opt = 1;
		if ((setsockopt((*it).getFd(), SOL_SOCKET,  SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0)
			throw std::runtime_error("setsockopt() failed");
	}
}

void Webserv::setNonblocking( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		int mem = 1;
		if ((ioctl((*it).getFd() , FIONBIO, (char *)&mem)) < 0)
			throw std::runtime_error("ioctl() failed");
	}
}

void Webserv::bindSocket( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		if ((bind((*it).getFd(), (struct sockaddr *)&(*it).getSockaddr(), 
				sizeof(struct sockaddr_in))) < 0)
			throw std::runtime_error("bind() failed");
	}
}

void Webserv::setListen( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		if ((listen((*it).getFd(), 0)) < 0)
			throw std::runtime_error("listen() failed");
	}
}

int Webserv::setFds( void )
{
	int max_sd = -1;
	int debug = 0;

	/*clear fd set*/
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	/*add master sockets*/
	std::list<class Server>::iterator srv = _servers.begin();	
	for	(; srv != _servers.end(); srv++)
	{
		FD_SET((*srv).getFd(), &readfds);
		debug++;
		if ((*srv).getFd() > max_sd)
			max_sd = (*srv).getFd();
	}

	/*add client socket*/
	std::list<class Client>::iterator clt = _clients.begin();	
	for (; clt != _clients.end(); clt++)
	{	
		if ((*clt).getFd() > 0)
		{
			FD_SET((*clt).getFd(), &readfds);
			debug++;
			if ((*clt).isReady())
				FD_SET((*clt).getFd(), &writefds);
			if ((*clt).getFd() > max_sd)
				max_sd = (*clt).getFd();
		}
	}
	return (max_sd);
}

void Webserv::acceptConnection(std::list<class Server>::iterator it, std::string set)
{
	int new_socket;
	int addrlen = sizeof(struct sockaddr_in);

	/*accept*/	
	new_socket = accept((*it).getFd(), (struct sockaddr *)&(*it).getSockaddr(), (socklen_t*)&addrlen);
	if (new_socket < 0)
		throw std::runtime_error("accept() failed");
	
	/*set non blocking*/
	int mem = 1;
	if ((ioctl(new_socket, FIONBIO, (char *)&mem)) < 0)
		throw std::runtime_error("ioctl() failed");

	/*set as reusable*/
	int opt = 1;
	if ((setsockopt((*it).getFd(), SOL_SOCKET,  SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0)
		throw std::runtime_error("setsockopt() failed");
	
	/*create new client*/
	_clients.push_back(Client(new_socket, (*it).getFd()));

	/*print info*/
	std::cout << BLUE << "===MASTER===" << RESET << std::endl;
	std::cout << YELLOW << "=> " << set << RESET << std::endl;
	std::cout << GREEN << "-> New connection" << RESET << std::endl;
	std::cout << GREEN << "  -On port : " << RESET << (*it).getPort() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << (*it).getFd() << std::endl;
	std::cout << GREEN << "  -Ip : " << RESET << inet_ntoa((*it).getSockaddr().sin_addr) << std::endl;
	std::cout << YELLOW << "  -> Client" << RESET << std::endl;
	std::cout << YELLOW << "     -Port : " << RESET << ntohs((*it).getSockaddr().sin_port) << std::endl;
	std::cout << YELLOW << "     -Socket fd : " << RESET << new_socket << std::endl;
	std::cout << YELLOW << "     -Ip : " << RESET << inet_ntoa((*it).getSockaddr().sin_addr) << std::endl;
}

std::list<class Client>::iterator Webserv::receiveRequest(std::list<class Client>::iterator it)
{
	int		ret = 1;
	char	buffer[4096];
	std::stringstream msg;
	int debug = 0;

	/*receive*/
	while (ret > 0)
	{
		memset(buffer, 0, 4096);
		ret = recv((*it).getFd(), &buffer, 4096, 0);
		if (ret > 0)
			msg.write((const char *)buffer, ret);
		debug += ret;
	}

	/*100 Continue*/
	if (msg.str().find("Expect") != std::string::npos)
	{
		ret = 1;
		sleep(2);
		while (ret > 0)
		{
			memset(buffer, 0, 4096);
			ret = recv((*it).getFd(), &buffer, 4096, 0);	
			if (ret > 0)
				msg.write((const char *)buffer, ret);
			debug += ret;
		}
	}	

	/*Check error*/
	if (ret == 0)
	{
		std::cout << BLUE << "\nCONNECTION CLOSED WITH FD: " << (*it).getFd() << RESET << std::endl << std::flush;
		return (_clients.erase(it));
	}

	/*link request to client*/
	(*it).setRequest(msg.str(), getServer((*it).getListen()));

	/*print info*/		
	std::cout << RED << "===CLIENT===" << RESET << std::endl;
	std::cout << YELLOW << "=> READ" << RESET << std::endl;
	std::cout << GREEN << "-> Receive" << RESET << std::endl;
	std::cout << GREEN << "  -listen fd : " << RESET << (*it).getListen() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << (*it).getFd() << std::endl;
	std::cout << GREEN << "  -Message : " << RESET << std::endl << msg.str().substr(0, 400) << std::endl;
	return (it);
}

std::list<class Client>::iterator Webserv::sendResponse(std::list<class Client>::iterator it)
{
	int	ret;

	/*send*/
	std::string buffer = (*it).getRequest().respond(getServer((*it).getListen()));
	ret = send((*it).getFd(), buffer.c_str(), buffer.size(), 0);

	/*Check error*/
	if (ret == 0)
	{
		close((*it).getFd());
		return (_clients.erase(it));
	}
	if (ret == -1)
		return (_clients.erase(it));

	/*print info*/		
	std::cout << RED << "===CLIENT===" << RESET << std::endl;
	std::cout << YELLOW << "=> WRITE" << RESET << std::endl;
	std::cout << GREEN << "-> Send" << RESET << std::endl;
	std::cout << GREEN << "  -listen fd : " << RESET << (*it).getListen() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << (*it).getFd() << std::endl;
	std::cout << GREEN << "  -Message : " << RESET << std::endl << buffer.substr(0, 400) << std::endl;

	/*close client*/
	close((*it).getFd());
	return (_clients.erase(it));
}

void Webserv::launch( void )
{
	int ret = 0;
	int max_sd;
	struct timeval tv;

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	std::cout << GREEN << "\n----------SELECT LOOP----------\n" << RESET << std::endl;
	while (true)
	{

		/*set fd*/
		max_sd = setFds();

		ret = select(max_sd + 1, &readfds, &writefds, NULL,  &tv);
		if (ret == 0)
			std::cout << BLUE << "\rWAITING FOR I/O OPERATION ..." << RESET << std::flush;
		else if (ret > 0)
		{	
			/*select*/
			std::cout << GREEN << "\n------------ SELECT ------------\n" << RESET << std::endl << std::flush;
			/*check client side*/
			std::list<class Client>::iterator clt = _clients.begin();
			for (; clt != _clients.end(); clt++)
			{
				/*set as write*/
				if (FD_ISSET((*clt).getFd(), &writefds) && (*clt).isReady())
					clt = sendResponse(clt);
			}

			clt = _clients.begin();
			for (; clt != _clients.end(); clt++)
			{
				/*set as read*/
				if (FD_ISSET((*clt).getFd(), &readfds)) {
					clt = receiveRequest(clt);
					break ;
				}
			}

			/*check master side*/
			std::list<class Server>::iterator srv = _servers.begin();
			for (; srv != _servers.end(); srv++)
			{
				/*set as read*/
				if (FD_ISSET((*srv).getFd(), &readfds)) {
					acceptConnection(srv, "READ");
					break ;
				}
			}
			std::cout << std::endl << std::flush;
		}
		else if (ret == -1)
		{
			/*relance the multiplexer*/
			std::list<class Client>::iterator clt = _clients.begin();
			for (; clt != _clients.end(); clt++)
				close((*clt).getFd());
			_clients.clear();
		}
	}
}
