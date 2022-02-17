#include "../include/lib.hpp"
#include "fcntl.h" ///pour test, a virer

/*reference*/
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

/*liste de status utile*/
//user agent ???
// 200 OK => tt reussi
// 201 => post a bien rajouter
// 204 => no content
// 300 => multiple choice (plein de ficher)
// 301 => move permently renvoi l'url dans le body
// 400 => bad request mais tres peu probable
// 403 => forbiden pas les droits (??? doute)
// 404 => not found
// 405 => method not allow, methode pas accepter par la location
// 431 => header trop large
// 501 => not implemented, par exemple HEAD
// 503 => service unavaible, si select crash (ou 500 Internal Server error ?)
// 505 => http version not supported



Request::Request( void )
{
	throw std::runtime_error("Request need buffer");
}

Request::Request(char* buffer)
{
	splitBuffer(buffer);
	parseType();
	parsePath();
	parseHttpVersion();
	parseAccept();
	//debug();
}

Request::Request(Request const& copy)
{
    *this = copy;
}

Request::~Request()
{
}

Request& Request::operator=(Request const& copy)
{
    if (this != &copy)
    {
		this->_buffer = copy._buffer;
		this->_type = copy._type;
    	this->_path = copy._path;
    	this->_accept = copy._accept;
		this->_filename = copy._filename;
		this->_httpver = copy._httpver;
		this->_root = copy._root;
		this->_status = copy._status;
    }
    return (*this);
}

/*parser*/
void Request::splitBuffer(char* buffer)
{
	std::string tmp(buffer);
	size_t index = 0;
	size_t ret = 0;

	while((ret = tmp.find("\n", index)) != std::string::npos)
	{
		this->_buffer.push_back(tmp.substr(index, ret - index));
		index = ret + 1;
	}
}

void Request::parseHttpVersion()
{
	size_t ret = 0;

	for (int i = 0; i < 2; i++)
		ret = _buffer[0].find(" ", ret + 1);
	this->_httpver = _buffer[0].substr(ret + 1, _buffer[0].size() - ret - 2);
}

void Request::parseType( void )
{
	size_t ret = 0;

	ret = _buffer[0].find(" ");
	this->_type = _buffer[0].substr(0 , ret);
}

void Request::parsePath( void )
{
	size_t ret = 0;

	ret = _buffer[0].find(" ");
	this->_path = _buffer[0].substr(ret + 1, _buffer[0].find(" ", ret + 1) - ret - 1);
	//if ((ret = this->_path.rfind("/")) != this->_path.size() - 1)
	// if ((ret = this->_root.rfind(".")) != std::string::npos)
	// {
	// 	ret = this->_root.rfind("/");
	// 	this->_filename = this->_root.substr(ret + 1, this->_path.size() - 1);
	// 	this->_root = this->_root.substr(0, ret + 1);
	// }
}

void Request::parseAccept( void )
{
	size_t line = 0;
	size_t index = 0;
	size_t ret = 0;

	while (line < this->_buffer.size())
	{
		if ((index = this->_buffer[line].find("Accept: ")) != std::string::npos)
			break;
		line++;
	}
	index += 8;
	if ( line == this->_buffer.size())
		throw std::runtime_error("parseAccept() failed");
	while((ret = _buffer[line].find(",", index)) != std::string::npos || (ret = _buffer[line].find(";", index)) != std::string::npos )
	{
		this->_accept.push_back(_buffer[line].substr(index, ret - index));
		index = ret + 1;
	}
}

void Request::parseFilename(class Server& srv)
{
	std::vector<class Location>::iterator	it = srv.getLocation().begin();
	size_t									ret = 0;

	for (; it != srv.getLocation().end(); it++)
	{
		if (this->_path.find((*it).getPath()) != std::string::npos && (*it).getPath().size() != 1)
		{
			// std::cout << YELLOW << "XDLOL\n";
			// std::cout << YELLOW << "location size : " << this->_root.size() << RESET << std::endl;
			this->_path = this->_path.substr((*it).getPath().size(), this->_path.size() - 1);
			this->_root = (*it).getRoot();
			if ((*it).getRoot().empty())
				this->_root = srv.getRoot();
			if ((ret = this->_path.rfind(".")) != std::string::npos)
			{
				ret = this->_path.rfind("/");
				this->_filename = this->_path.substr(ret + 1, this->_path.size() - ret - 1);
				this->_path = this->_root.substr(0, ret);
			}
			// else if (!(*it).getIndex().empty())
			// 	this->_filename = "/" + (*it).getIndex();
			//else
			// {
			// 	this->_filename = srv.getDefaultPage();
			// }
			break ;
		}
	}
	if (it == srv.getLocation().end())
	{
		//if (this->_path.size() > 1)
		if ((ret = this->_path.rfind("/")) != 0)
			this->_path = this->_path.substr(1, this->_path.size() - 1);
		this->_root = srv.getRoot();
		if ((ret = this->_path.rfind(".")) != std::string::npos)
		{
			ret = this->_path.rfind("/");
			this->_filename = this->_path.substr(ret + 1, this->_path.size() - ret - 1);
			this->_path = this->_path.substr(0, ret + 1);
		}
	}
	if (this->_filename.empty())
	{
		if (it != srv.getLocation().end() && !(*it).getIndex().empty())
			this->_filename = (*it).getIndex();
		if (this->_filename.empty())
			this->_filename = srv.getIndex();
	}
}

/*setter
void Request::setType(std::string type)
{
	this->_type = type;
}

void Request::setPath(std::string path)
{
	this->_path = path;
}

void Request::setAccept(std:;string accept)
{
	this->_accept = accept;
}
*/

/*getter*/
std::vector<std::string> Request::getBuffer( void ) const
{
	return (this->_buffer);
}

std::string Request::getType( void ) const
{
	return (this->_type);
}

std::string Request::getPath( void ) const
{
	return (this->_path);
}

std::vector<std::string> Request::getAccept( void ) const
{
	return (this->_accept);
}

std::string Request::getHttpver( void ) const
{
	return (this->_httpver);
}

std::string Request::getRoot( void ) const
{
	return (this->_root);
}

std::string Request::getFilename( void ) const
{
	return (this->_filename);
}

std::string Request::getStatus( void ) const
{
	return (this->_status);
}

/*check*/
std::string Request::checkContent(class Server &srv)
{
	(void)srv;
	std::ifstream t("." + this->_root + this->_path + this->_filename); //faux
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string body = buffer.str();

	//std::cout << "body : " << body << std::endl;

	if (!t.is_open())
	{
		this->_status = "HTTP/1.1 404 Not found\n";
		return ("");
	}
	if (buffer.str().empty())
	{
		this->_status = "HTTP/1.1 204 No Content\n";
		return ("");
	}
	t.close();
	return (body);
}

std::vector<class Location>::iterator Request::checkPath(class Server &srv)
{
	std::vector<class Location>::iterator it = srv.getLocation().begin();

	for ( ; it != srv.getLocation().end(); it++)
	{
		if ((*it).getPath() == this->_root)
			return (it);
	}
	return (srv.getLocation().end());	
}

bool Request::checkMethod(Location& location)
{
	if (this->_type == "GET")
		return (location.getGetMethod());
	else if (this->_type == "POST")
		return (location.getPostMethod());
	else if (this->_type == "DELETE")
		return (location.getDeleteMethod());
	else
		return (false);
}

// 200 OK => tt reussi
// 201 => post a bien rajouter
// 204 => no content
// 300 => multiple choice (plein de ficher)
// 400 => bad request mais tres peu probable
// 431 => header trop large
// 503 => service unavaible, si select crash (ou 500 Internal Server error ?)

void Request::checkStatus(class Server &srv)
{
	std::vector<class Location>::iterator it = checkPath(srv);

	if (this->_httpver != "HTTP/1.1")
		this->_status = "HTTP/1.1 505 HTTP Version not supported\n";
	else if (this->_type != "GET" && this->_type != "POST" && this->_type != "DELETE")
		this->_status = "HTTP/1.1 501 Not Implemented\n";
	else if ((it == srv.getLocation().end()))
		this->_status = "HTTP/1.1 404 Not Found\n";
	// else if ((ret = checkContent(srv)) != "OK")
	// 	this->_status = 
	else if (!checkMethod(*it))
		this->_status = "HTTP/1.1 405 Method Not Allowed\n";
	//else if ()
	//	this->_status = "HTTP/1.1 301 Moved Permanently\n";
}

std::string Request::checkContentType()
{
	size_t		ret = this->_filename.find(".");
	std::string	ext = this->_filename.substr(ret + 1, this->_filename.size() - 1);

	//std::cout << YELLOW << "ext : " << ext << RESET << std::endl;

	std::vector<std::string>::iterator it = this->_accept.begin();

	for (; it != this->_accept.end(); it++)
	{
		if ((*it).find(ext))
			return ("Content-Type: " + (*it) + "\n");
	}
	return (""); /////// type de contenu pas accepté /// faut changer quoi
}

/*respond*/
std::string Request::respond(class Server& srv)
{
	parseFilename(srv);
	std::string response = "";
	std::string content = "";

	// if (response.find("200") != std::string::npos)
	// {
		content = checkContent(srv);
		if (this->_status.empty())
		{
			//std::cout << GREEN << "Content : " << content << RESET << std::endl;
			this->_status = "HTTP/1.1 200 OK\n";
			response += this->_status + checkContentType() + "Content-length: " + std::to_string(content.size()) + "\n\n" + content;
			//std::cout << YELLOW << "response : " << response << RESET << std::endl;
			//std::cout << YELLOW << "status : " << this->_status << RESET << std::endl;
		}
		else
			response = this->_status;
	// }
	debug();
	return (response);
}

/*debug*/
void Request::debug( void )
{
	size_t i = 0;

	std::cout << RED << "TYPE : " << RESET << this->getType() << std::endl;
	std::cout << RED << "PATH : " << RESET << this->getPath() << std::endl;
	std::cout << RED << "HTTP : " << RESET <<this->getHttpver() << std::endl;
	std::cout << RED << "FILE NAME : " << RESET << this->getFilename() << std::endl;
	std::cout << RED << "ROOT : " << RESET << this->getRoot() << std::endl;
	std::cout << RED << "ACCEPT : " << RESET << std::endl;
	while ( i < _accept.size())
	{
		std::cout << "   " << this->getAccept()[i] << std::endl;
		i++;
	}
	std::cout << RED << "BUFFER : " << RESET << std::endl;
	i = 0;
	while ( i < _buffer.size())
	{
		std::cout << "   " << this->getBuffer()[i] << std::endl;
		i++;
	}
}
