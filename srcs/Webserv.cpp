/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: namenega <namenega@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:09:58 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/02/18 17:19:35 by namenega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

Webserv::Webserv() : on(1)
{
	throw std::runtime_error("no config file");
}

Webserv::Webserv( std::vector<class Server> const& servers ) : on(1)
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

Webserv::Webserv( Webserv const& copy )
{
	*this = copy;
}

Webserv& Webserv::operator=(Webserv const& copy)
{
	if (this != &copy)
	{
		this->on = copy.on;
		this->_servers = copy._servers;
		this->_clients = copy._clients;
		this->readfds = copy.readfds;
		this->writefds = copy.writefds;
	}
	return (*this);
}
		
Webserv::~Webserv( void )
{	/* voir destructor de client et server*/
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
		if ((ioctl((*it).getFd() , FIONBIO, (char *)&mem)) < 0) //&ON A VERIF!!!!!!
			throw std::runtime_error("ioctl() failed");
	}
}

void Webserv::bindSocket( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		if ((bind((*it).getFd(), (struct sockaddr *)&(*it).getSockaddr(), 
				sizeof(struct sockaddr_in))) < 0) {
			std::cout << BLUE << "BIND ERR : " << std::strerror(errno) << RESET << std::endl;
			throw std::runtime_error("bind() failed : " + std::to_string((*it).getFd()));
		}
	}
}

void Webserv::setListen( void )
{
	std::list<class Server>::iterator it = _servers.begin();

	for ( ; it != _servers.end(); it++)
	{
		if ((listen((*it).getFd(), 32)) < 0) //32 pending connection maybe too much ??
			throw std::runtime_error("listen() failed");
	}
}

int Webserv::setFds( void )
{
	int max_sd = -1;

	/*clear fd set*/
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	/*add master sockets*/
	std::list<class Server>::iterator srv = _servers.begin();	
	for	( ; srv != _servers.end(); srv++)
	{
		FD_SET((*srv).getFd(), &readfds);
		FD_SET((*srv).getFd(), &writefds);
		if ((*srv).getFd() > max_sd)
			max_sd = (*srv).getFd();
	}

	/*add client socket*/
	std::list<class Client>::iterator clt = _clients.begin();	
	for ( ; clt != _clients.end(); clt++)
	{	
		if ((*clt).getFd() > 0)
		{
			FD_SET((*clt).getFd(), &readfds);
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

bool Webserv::receiveRequest(std::list<class Client>::iterator it)
{
	char buffer[30001]; // change 3000 par max body size ?;

	/*receive*/
	memset(buffer, 0, 30001);
	if((recv((*it).getFd(), buffer, 30000, 0)) < 0)
		throw std::runtime_error("recv() failed");
	
	/*link request to client*/
	if (buffer[0] != 0)
		(*it).setRequest(Request(buffer, getServer((*it).getListen())));
	else
	{
		close((*it).getFd());
		_clients.erase(it);
		return (false);
	}
	/*print info*/		
	std::cout << RED << "===CLIENT===" << RESET << std::endl;
	std::cout << YELLOW << "=> READ" << RESET << std::endl;
	std::cout << GREEN << "-> Receive" << RESET << std::endl;
	std::cout << GREEN << "  -listen fd : " << RESET << (*it).getListen() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << (*it).getFd() << std::endl;
	std::cout << GREEN << "  -Message : " << RESET << std::endl << buffer;
	return (true);
}

std::list<class Client>::iterator Webserv::sendResponse(std::list<class Client>::iterator it)
{
	/*debug*/
	//std::string buffer = "HTTP/1.1 200 OK\nContent-type: text/plain\nContent-Length: 12\n\nHello world!";
	/*send*/
	std::string buffer = (*it).getRequest().respond(getServer((*it).getListen()));
	if ((send((*it).getFd(), buffer.c_str(), buffer.size(), 0)) <= 0)
	//if((send((*it).getFd(), greets.c_str(), greets.size(), 0)) <= 0)//TODO send request
		throw std::runtime_error("send() failed");

	/*print info*/		
	std::cout << RED << "===CLIENT===" << RESET << std::endl;
	std::cout << YELLOW << "=> WRITE" << RESET << std::endl;
	std::cout << GREEN << "-> Send" << RESET << std::endl;
	std::cout << GREEN << "  -listen fd : " << RESET << (*it).getListen() << std::endl;
	std::cout << GREEN << "  -Socket fd : " << RESET << (*it).getFd() << std::endl;
	std::cout << GREEN << "  -Message : " << RESET << std::endl << buffer.substr(0, 100) << std::endl; //TODO A CHANGER
	// printf("buffer : ", );

	/*close client*/
	close((*it).getFd());
	return (_clients.erase(it));
}

void Webserv::launch( void )
{
	int ret = -1;
	int max_sd;

	std::cout << GREEN << "\n----------SELECT LOOP----------\n" << RESET << std::endl;
	while (true)
	{
		/*set fd*/
		max_sd = setFds();

		/*select*/
		// int                n = 0;
    	// std::string        waitingPoint[3] = {".  ", ".. ", "..."};
    	// while ((ret = select(max_sd + 1, &readfds, &writefds, NULL, NULL)) <= 0)
		// {
        // 	std::cout << "\rWaiting on a connection " << waitingPoint[n++] << std::flush;
        // 	if (n == 3)
        //     	n = 0;
        // 	sleep(1);
    	// }
		ret = select(max_sd + 1, &readfds, &writefds, NULL, NULL);
	
		if (ret > 0)
		{	
			/*check client side*/
			std::list<class Client>::iterator clt = _clients.begin();
			for ( ; clt != _clients.end(); clt++)
			{
				/*set as read*/
				if (FD_ISSET((*clt).getFd(), &readfds))
				{
					if (!receiveRequest(clt))
						break ;
				}
				/*set as write*/
				if (FD_ISSET((*clt).getFd(), &writefds) && (*clt).isReady())
				{
					clt = sendResponse(clt);
					//break; //TODO A ENLEVER ET RECUP IT DE SENDRESPONSE
				}
			}

			/*check master side*/
			std::list<class Server>::iterator srv = _servers.begin();
			for ( ; srv != _servers.end(); srv++)
			{
				/*set as read*/
				if (FD_ISSET((*srv).getFd(), &readfds))
					acceptConnection(srv, "READ");
			}
			//std::cout << GREEN << "\n------------END LOOP-----------\n" << RESET << std::endl;
			//usleep(200);
		}
		else
		{
			/*TODO relance the multiplexer*/
			throw std::runtime_error("select() failed");
		}
	}
}
