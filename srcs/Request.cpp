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

void Request::parseType( void )
{
	size_t ret = 0;

	ret = _buffer[0].find(" "); ///Invalid read of size 8 if buffer vide
	this->_type = _buffer[0].substr(0 , ret);
}

void Request::parsePath( void )
{
	size_t ret = 0;

	ret = _buffer[0].find(" ");
	this->_path = _buffer[0].substr(ret + 1, _buffer[0].find(" ", ret + 1) - ret - 1);
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

/*check*/
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

std::string  Request::checkStatus(class Server &srv)
{

}

/*respond*/
std::string Request::createResponse(class Server& srv)
{
	std::string status:
	std::string content_type;
	std::string content_length;
	std::string content;

	status = checkStatus(srv);
	return (status + content_type + content_length + content);
}
	
std::string Request::respond(class Server& srv)
{
	std::vector<class Location>::iterator it = srv.getLocation().begin();

	/*debug*/
	std::string badMethod = "HTTP/1.1 300\nContent-type: text/plain\nContent-Length: 11\n\nBad Method.";
	std::string greets = "HTTP/1.1 200 OK\nContent-type: text/plain\nContent-Length: 12\n\nHello world!";
	std::string notfound = "HTTP/1.1 404\nContent-type: text/plain\nContent-Length: 13\n\n404 Not Found";
	
	for ( ; it != srv.getLocation().end(); it++)
	{
		if ((*it).getPath() == this->_path)
		{
			if (!checkMethod(*it))
				return (badMethod);
			return (greets);
		}
	}
	return (notfound);
}

/*debug*/
void Request::debug( void )
{
	size_t i = 0;

	std::cout << RED << "TYPE : " << RESET << this->getType() << std::endl;
	std::cout << RED << "PATH : " << RESET << this->getPath() << std::endl;
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
