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
	debug();
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
		this->_location = copy._location;
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
	this->_location = _buffer[0].substr(ret + 1, _buffer[0].find(" ", ret + 1) - ret - 1);
	//if ((ret = this->_path.rfind("/")) != this->_path.size() - 1)
	if ((ret = this->_location.rfind(".")) != std::string::npos)
	{
		ret = this->_location.rfind("/");
		this->_filename = this->_location.substr(ret + 1, this->_path.size() - 1);
		this->_location = this->_location.substr(0, ret + 1);
	}
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

std::string Request::getLocation( void ) const
{
	return (this->_location);
}

std::string Request::getFilename( void ) const
{
	return (this->_filename);
}

/*check*/

std::string Request::checkContent(class Server &srv)
{
	char* buffer[10];
	int ret = 0;
	int fd = open((srv.getRoot() + this->_path + this->_filename).c_str(), O_RDONLY);

	if (fd < 0)
		return ("HTTP/1.1 404 Not found");
	memset(buffer, 0, 10);
	ret = read(fd, buffer, 9);
	if (buffer[0] == 0)
		return ("HTTP/1.1 204 No Content\n");
	else
		return ("OK");
}

std::vector<class Location>::iterator Request::checkPath(class Server &srv)
{
	std::vector<class Location>::iterator it = srv.getLocation().begin();

	for ( ; it != srv.getLocation().end(); it++)
	{
		if ((*it).getPath() == this->_path)
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

std::string  Request::checkStatus(class Server &srv)
{
	std::string ret;
	std::vector<class Location>::iterator it = checkPath(srv);

	/*find location*/
	for ( ; it != srv.getLocation().end(); it++)
	{
		if ((*it).getPath() == this->_path)
			break ;
	}

	if (this->_httpver != "HTTP/1.1")
		return ("HTTP/1.1 505 HTTP Version not supported\n");
	else if (this->_type != "GET" && this->_type != "POST" && this->_type != "DELETE")
		return ("HTTP/1.1 501 Not Implemented\n");
	else if ((it == srv.getLocation().end()))
	 	return ("HTTP/1.1 404 Not Found\n");
	else if ((ret = checkContent(srv)) != "OK")
		return (ret);
	else if (!checkMethod(*it))
		return ("HTTP/1.1 405 Method Not Allowed\n");
	//else if ()
	//	return ("HTTP/1.1 301 Moved Permanently\n");
	else
		return ("HTTP/1.1 200 OK\n");
	return (ret);
}

/*respond*/
std::string Request::respond(class Server& srv)
{
	std::string response = checkStatus(srv);
	std::string content;

	if (response.find("200") != std::string::npos)
	{
		//content = getContent();
		//response += checkContentType() + "Content-length: " + to_string(content.size()) + content;
	}
	return (response);
}

/*debug*/
void Request::debug( void )
{
	size_t i = 0;

	std::cout << RED << "TYPE : " << RESET << this->getType() << std::endl;
	std::cout << RED << "PATH : " << RESET << this->getPath() << std::endl;
	std::cout << RED << "HTTP : " << RESET <<this->getHttpver() << std::endl;
	std::cout << RED << "File name : " << RESET << this->getFilename() << std::endl;
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
