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

Request::Request(char* buffer, Server& srv)
{
	splitBuffer(buffer);
	parseType();
	parsePath();
	parseHttpVersion();
	parseLocation(srv);
	parseFilename(srv);
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
	this->_path = _buffer[0].substr(ret + 1, _buffer[0].find(" ", ret + 1) - ret - 1);
}

void parseLocation(Server& srv)
{
	std::vector<class Location>::iterator	it = srv.getLocation().begin();
	size_t									ret = 0;

	this->_location = (*it);
	for (; it != srv.getLocation().end(); it++)
	{
		if (this->_path.find((*it).getPath()) != std::string::npos && (*it).getPath().size() != 1)
		{
			this->_root = (*it).getRoot();
			this->_path = this->_path.substr((*it).getPath().size(), this->_path.size() - 1);
			this->_location = (*it);
			break ;
		}
	}
	if (this->_root.empty())
		this->_root = srv.getRoot();
}

void Request::parseFilename(class Server& srv)
{
	if ((ret = this->_path.rfind(".")) != std::string::npos)
	{
		ret = this->_path.rfind("/");
		this->_filename = this->_path.substr(ret, this->_path.size() - ret);
		this->_path = this->_root.substr(0, ret);
	}
	else
	{
		////////// auto index
		if (!this->_location->getIndex().empty())
			this->_filename = "/" + (*it).getIndex();
		else
			this->_filename = "/" + srv.getIndex();
	}
}

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

/*	check */
std::vector<class CGI>::iterator	Request::checkCGI(Server & server)
{
	size_t		ret = this->_filename.find(".");
	std::string	ext = this->_filename.substr(ret + 1, this->_filename.size() - 1);
	std::vector<class CGI>::iterator it = server.getCGI().begin();

	for (; it != server.getCGI().end(); it++)
	{
		if ((*it).getExtension().find(ext) != std::string::npos)
			return (it);
	}
	return (it);
}

std::string Request::checkContent(class Server &srv)
{
	(void)srv;
	std::string body;
	std::ifstream t("." + this->_root + this->_path + this->_filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	// body = buffer.str();

	if (!t.is_open())
		this->_status = "HTTP/1.1 404 Not found\n";
	// if (buffer.str().empty())
	// 	this->_status = "HTTP/1.1 204 No Content\n";
	t.close();
	std::vector<class CGI>::iterator	it;
	if ((it = checkCGI(srv)) != srv.getCGI().end())
	{
		(*it).execute(*this, srv);
		body = (*it).getBodyVar();
	}
	else
		body = buffer.str();
	return (body);
}

std::string Request::checkContentType()
{
	size_t		ret = this->_filename.find(".");
	std::string	ext = this->_filename.substr(ret + 1, this->_filename.size() - 1);
	std::string	type;

	std::vector<std::string>::iterator it = this->_accept.begin();

	for (; it != this->_accept.end(); it++)
	{
		if ((*it).find(ext) != std::string::npos)
			return ("Content-Type: " + (*it) + "\n");
		if ((ret = (*it).find("/*")) != std::string::npos) {
			type = (*it).substr(0, ret) + ext;
			return ("Content-Type: " + type + "\n");
		}
	}
	return (""); /////// type de contenu pas accepté /// faut changer quoi
}

/*respond*/

std::string Request::errorPage(std::string str)
{
	size_t				ret = 0;
	std::string			error;
	std::ifstream		t("./www/error.html");
	std::stringstream	buffer;
	buffer << t.rdbuf();
	error = buffer.str();

	ret = error.find("STATUS");
	error.erase(ret, ret + 6);
	if (str.empty())
		error.insert(ret, this->_status);
	else
		error.insert(ret, str);
	return (error);
}

//else if ()
//	this->_status = "HTTP/1.1 301 Moved Permanently\n";
std::string Request::respond(class Server& srv)
{
	if (this->_httpver != "HTTP/1.1")
		return (errorPage("HTTP/1.1 505 HTTP Version not supported\n");
	else if ((this->_type == "GET" && !this->_location->getGetMethod()) ||
			(this->_type == "POST" && !this->_location->getPostMethod()) ||
			(this->_type == "DELETE" && !this->_location->getDeleteMethod()))
		return (errorPage("HTTP/1.1 405 Method Not Allowed\n"));
	else if (this->_type == "GET")
		return (GETRequest(srv));
	else if (this->_type == "POST")
		return (POSTRequest(srv))
	else if (this->_type == "DELETE")
		return (DELETERequest(srv))
	else
		return (errorPage("HTTP/1.1 501 Not Implemented\n"))
}

/*GET request*/
std::string Request::GETRequest(Server& srv)
{
	parseAccept();

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
	while((ret = _buffer[line].find(",", index)) != std::string::npos ||
			(ret = _buffer[line].find(";", index)) != std::string::npos)
	{
		this->_accept.push_back(_buffer[line].substr(index, ret - index));
		index = ret + 1;
	}
}

/*Post request*/

/*Delete request*/
void Request::DELETERequest()
{
	parseFilename(srv);
	if (this->_filename != "/index.html")
	{
		std::ifstream t("." + this->_root + this->_path + this->_filename);
		if (t.is_open())
		{
			t.close();
			remove("." + this->_root + this->_path + this->_filename);
		}
		else
			this->_status = "HTTP/1.1 404 Not found\n";
	}
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
