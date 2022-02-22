#include "../include/lib.hpp"
#include "fcntl.h" ///pour test, a virer

/*reference*/
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

/*liste de status utile*/
//user agent ???
// 200 OK => tt reussi--------------------------------------------------------------V
// 201 => post a bien rajouter
// 204 => no content----------------------------------------------------------------V
// 300 => multiple choice (plein de ficher)-----------------------------------------X
// 301 => move permently renvoi l'url dans le body
// 400 => bad request mais tres peu probable
// 403 => forbiden pas les droits (??? doute)
// 404 => not found-----------------------------------------------------------------V
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
		this->_bad_status = copy._bad_status;
		this->_location = copy._location;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////// Parser ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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
	std::list<std::string>::iterator it = this->_buffer.begin();
	size_t ret = 0;

	for (int i = 0; i < 2; i++)
		ret = (*it).find(" ", ret + 1);
	this->_httpver = (*it).substr(ret + 1, (*it).size() - ret - 2);
}

void Request::parseType( void )
{
	size_t ret = 0;

	ret = this->_buffer.begin()->find(" ");
	this->_type = this->_buffer.begin()->substr(0 , ret);
}

void Request::parsePath( void )
{
	size_t ret = 0;

	ret = this->_buffer.begin()->find(" ");
	this->_path = this->_buffer.begin()->substr(ret + 1, this->_buffer.begin()->find(" ", ret + 1) - ret - 1);
}

void Request::parseLocation(Server& srv)
{
	std::vector<class Location>::iterator	it = srv.getLocation().begin();

	this->_location = &(*it);
	for (; it != srv.getLocation().end(); it++)
	{
		if (this->_path.find((*it).getPath()) != std::string::npos && (*it).getPath().size() != 1)
		{
			this->_root = (*it).getRoot();
			this->_path = this->_path.substr((*it).getPath().size(), this->_path.size() - 1);
			this->_location = &(*it);
			break ;
		}
	}
	if (this->_root.empty())
		this->_root = srv.getRoot();
}

void Request::parseFilename(class Server& srv)
{
	size_t ret = 0;
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
			this->_filename = "/" + this->_location->getIndex();
		else
			this->_filename = "/" + srv.getIndex();
	}
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Getters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::list<std::string> Request::getBuffer( void ) const
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

std::list<std::string> Request::getAccept( void ) const
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

bool Request::getBadStatus( void ) const
{
	return (this->_bad_status);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////// CGI /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Respond ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
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
	// if (str.empty())
	// 	error.insert(ret, this->_bad_status);
	//else
		error.insert(ret, str);
	return (str + "Content-Type: text/html\nContent-length: " + std::to_string(error.size()) + "\n\n" + error);
}

//else if ()
//	this->_status = "HTTP/1.1 301 Moved Permanently\n";
std::string Request::respond(class Server& srv)
{
	if (this->_httpver != "HTTP/1.1")
		return (errorPage("HTTP/1.1 505 HTTP Version not supported\n"));
	else if ((this->_type == "GET" && !this->_location->getGetMethod()) ||
			(this->_type == "POST" && !this->_location->getPostMethod()) ||
			(this->_type == "DELETE" && !this->_location->getDeleteMethod()))
		return (errorPage("HTTP/1.1 405 Method Not Allowed\n"));
	else if (this->_type == "GET")
		return (GETRequest(srv));
	else if (this->_type == "POST")
		return (POSTRequest(srv));
	else if (this->_type == "DELETE")
		return (DELETERequest(srv));
	else
		return (errorPage("HTTP/1.1 501 Not Implemented\n"));
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////// GET request //////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Request::GETRequest(Server& srv)
{
	parseAccept();
	return (GETResponse(srv));
}

void Request::parseAccept( void )
{
	size_t index = 0;
	size_t ret = 0;
	std::list<std::string>::iterator it = this->_buffer.begin();

	for (; it != this->_buffer.end(); it++)
	{
		if ((index = (*it).find("Accept: ")) != std::string::npos)
			break;
	}
	index += 8;
	if (it == this->_buffer.end())
		throw std::runtime_error("parseAccept() failed");
	while((ret = (*it).find(",", index)) != std::string::npos ||
			(ret = (*it).find(";", index)) != std::string::npos)
	{
		this->_accept.push_back((*it).substr(index, ret - index));
		index = ret + 1;
	}
}

std::string Request::GETResponse(class Server &srv)
{
	(void)srv;
	std::string body;
	//std::string contentType;
	std::ifstream t("." + this->_root + this->_path + this->_filename);
	std::stringstream buffer;
	buffer << t.rdbuf();

	if (t.is_open() && buffer.str().empty())
	{
		t.close();
		return (errorPage("HTTP/1.1 204 No Content\n"));
	}
	if (!t.is_open())
	{
		t.close();
		return (errorPage("HTTP/1.1 404 Not found\n"));
	}
	t.close();

	std::vector<class CGI>::iterator	it;
	
	if ((it = checkCGI(srv)) != srv.getCGI().end())
	{
		(*it).execute(*this, srv);
		body = (*it).getBodyVar();
	}
	else
		body = buffer.str();
	//contentType = contentType();
	// if (contentType.empty())
	// 	return ()
	return ("HTTP/1.1 200 OK\n" + contentType() + "Content-length: " + std::to_string(body.size()) + "\n\n" + body);
}

std::string Request::contentType()
{
	size_t		ret = this->_filename.find(".");
	std::string	ext = this->_filename.substr(ret + 1, this->_filename.size() - 1);
	std::string	type;

	std::list<std::string>::iterator it = this->_accept.begin();

	for (; it != this->_accept.end(); it++)
	{
		if ((*it).find(ext) != std::string::npos)
			return ("Content-Type: " + (*it) + "\n");
		if ((ret = (*it).find("/*")) != std::string::npos) {
			type = (*it).substr(0, ret + 1) + ext;
			return ("Content-Type: " + type + "\n");
		}
	}
	return (""); /////// type de contenu pas accepté /// faut changer quoi
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////// POST request /////////////////////////////////
///////////////////////////////////////////////////////////////////////////

std::string Request::POSTRequest(Server& srv)
{
	(void)srv;
	return ("POST Request");
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////// DELETE request ////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Request::DELETERequest(Server& srv)
{
	parseFilename(srv);
	if (this->_filename != "/index.html")
	{
		std::ifstream t("." + this->_root + this->_path + this->_filename);
		if (t.is_open())
		{
			t.close();
			std::remove(("." + this->_root + this->_path + this->_filename).c_str());
		}
		else
			return (errorPage("HTTP/1.1 404 Not found\n"));
	}
	return ("DELETE Request");
}

/*debug*/
void Request::debug( void )
{
	std::list<std::string>::iterator it;

	std::cout << RED << "TYPE : " << RESET << this->getType() << std::endl;
	std::cout << RED << "PATH : " << RESET << this->getPath() << std::endl;
	std::cout << RED << "HTTP : " << RESET <<this->getHttpver() << std::endl;
	std::cout << RED << "FILE NAME : " << RESET << this->getFilename() << std::endl;
	std::cout << RED << "ROOT : " << RESET << this->getRoot() << std::endl;
	std::cout << RED << "ACCEPT : " << RESET << std::endl;
	for (it = this->_accept.begin(); it != this->_accept.end(); it++)
		std::cout << "   " << (*it) << std::endl;
	std::cout << RED << "BUFFER : " << RESET << std::endl;
	for (it = this->_buffer.begin(); it != this->_buffer.end(); it++)
		std::cout << "   " << (*it) << std::endl;
}
