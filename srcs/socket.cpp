/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 13:05:45 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/13 17:34:10 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Reference
 * https://www.binarytides.com/multiple-socket-connections-fdset-select-linux/
 * https://programmer.help/blogs/multiplexer-poll.html
 * https://notes.shichao.io/unp/ch6/
 * https://linuxhint.com/use-poll-system-call-c/
 */

class Webserv
{
	public:
		Webserv() {};
		~Webserv() {};

		/*Getter*/
		std::list<Server> &getServers( void ) { return _servers; }
		struct pollfd* getPollfd( void ) { return _fds; }

	private:
		std::list<Server> _servers;
		struct pollfd *_fds;

};

void createSocket(Webserv &webserv)
{
	int on = 1;
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = webserv.getServers().begin();
	end = webserv.getServers().end();
	
	for( ; bgin != end; bgin++) 
	{
		if ((webserv.getPollfd()[index].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) //create socket
			throw std::exception("socket() failed");
		webserv.getPollfd()[index].events = POLLIN | POLLOUT;
		
		if ((setsosockopt(webserv.getPollfd()[index].fd, SOL_SOCKET,  SO_REUSEADDR, // allow to be reusable
                  (char *)&on, sizeof(on))) < 0)
			throw std::exception("setsockopt() failed");

		index++;
	}
}

void setNonblocking(Webser &webserv)
{
	int on = 1;
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = webserv.GetServers().begin();
	end = webserv.GetServers().end();
	
	for( ; bgin != end; bgin++)
	{
		if ((ioctl(webserv.getPollfd()[index].fd , FIONBIO, (char *)&on)) < 0)
			throw std::exception("ioctl() failed");

		index++;
	}
}

void bindSocket(Webserv &webserv)
{
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = webserv.GetServers().begin();
	end = webserv.GetServers().end();

	for( ; bgin != end; bgin++)
	{
		if ((bind(webserv.getPollfd()[index].fd , webserv.getServers()[index].getSockaddr(), 
				sizeof(struct sockaddr_in) < 0)
			throw std::exception("bind() failed");

		index++;
	}
}

void setListen(Webserv &webserv)
{
	int index = 0;
	std::list<Server *>::iterator bgin;
	std::list<Server *>::iterator end;

	bgin = webserv.GetServers().begin();
	end = webserv.GetServers().end();

	for( ; bgin != end; bgin++)
	{
		if ((listen(webserv.getPollfd()[index].fd , 32) < 0) //32 pending connection possible maybe too much ??
			throw std::exception("listen() failed");

		index++;
	}
}

void initializePoll(Webserv &webserv)
{
	try
	{
		createSocket(webserv);
		setNonblocking(webserv);
		bindSocket(webserv);
		setListen(webserv);
	}
	catch(std::exception & e)
	{
		write(2, e.what(). strlen(e.what()));
		//closeFd(webserv)
		throw std::exception("Initialization failure");
	}
}
