/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: namenega <namenega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 10:48:23 by namenega          #+#    #+#             */
/*   Updated: 2022/03/14 10:48:23 by namenega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"
#include "fcntl.h" ///pour test, a virer

/*reference*/
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

Request::Request( void )
{
	throw std::runtime_error("Request need buffer");
}

Request::Request(char* buffer, Server& srv)
{
	if (srv.getErrorPage().empty())
		this->_errorpage = "./www/error.html";
	else
		this->_errorpage = "." + srv.getErrorPage();
	splitBuffer(buffer);
	parseType();
	parsePath();
	parseHttpVersion();
	parseFilename();
	parseLocation(srv);
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
		this->_body = copy._body;
		this->_location = copy._location;
		this->_errorpage = copy._errorpage;
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
 
	tmp.erase(std::remove(tmp.begin(), tmp.end(), '\r'), tmp.end());
	while (index < tmp.size() - 1)
	{
		if ((ret = tmp.find("\n", index)) == std::string::npos)
			ret = tmp.size() - 1;
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
	this->_httpver = (*it).substr(ret + 1, (*it).size() - ret - 1);
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

void Request::parseLocation(Server& srv)		 //////// Probleme : si filename = au nom d'une location -> server crash
{
	std::vector<class Location>::iterator	it = srv.getLocation().begin();
	size_t									ret = 0;

	this->_location = &(*it);
	for (; it != srv.getLocation().end(); it++)
	{
		if (((ret = this->_path.find((*it).getPath())) != std::string::npos) && ((*it).getPath().size() != 1) /*&& (ret < this->_path.rfind("/"))*/)
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

void Request::parseFilename()
{
	size_t ret = 0;
	if ((ret = this->_path.rfind(".")) != std::string::npos)
	{
		ret = this->_path.rfind("/");
		this->_filename = this->_path.substr(ret, this->_path.size() - ret);
		this->_path = this->_path.substr(0, ret);
	}
}

void Request::parseBody()
{
	std::list<std::string>::iterator	it = this->_buffer.begin();
	bool								chunck = false;

	debug();
	for (; it != this->_buffer.end(); it++)
	{
		if ((*it).find("Transfer-Encoding: chunked") != std::string::npos)
		{
			chunck = true;
			break;
		}
	}
	it = this->_buffer.begin();
	while (it != this->_buffer.end() && (*it).size() != 0)
		it++;
	if (it != this->_buffer.end())
		it++;
	for (; it != this->_buffer.end(); it++)
		this->_body.append((*it) + "\n");
	if (chunck)
		dechunk();  //https://fr.wikipedia.org/wiki/Chunked_transfer_encoding
}

void Request::dechunk()
{
	std::string buffer;
	int y, i = 0;
	std::stringstream stream;

	while (this->_body[i])
	{
		for (; this->_body[i] && this->_body[i] != '\n'; i++)
		{
			stream << this->_body[i];
			stream >> std::hex >> y;
		}
		if (y == 0)
			break;
		for (; this->_body[i] && i < y; i++)
			buffer.push_back(this->_body[i]);
		while (this->_body[i] && this->_body[i] != '\n')
			i++;
		i++;
	}
	this->_body = buffer;
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

std::string Request::getBody( void ) const
{
	return (this->_body);
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
	// if (srv.getErrorPage().empty())			  //// default error_page
	std::ifstream		t(this->_errorpage.c_str());
	if (!t.is_open())
		t.open("./www/error.html");
	// else
	// 	std::ifstream 		t("root" + "/");
	std::stringstream	buffer;
	buffer << t.rdbuf();
	t.close();
	error = buffer.str();

	ret = error.find("STATUS");
	error.erase(ret, ret + 6);
	// if (str.empty())
	// 	error.insert(ret, this->_bad_status);
	//else
		error.insert(ret, str);
	return (str + "Content-Type: text/html\nContent-length: " + ntostring(error.size()) + "\n\n" + error);
}

//else if ()
//	this->_status = "HTTP/1.1 301 Moved Permanently\n";
std::string Request::respond(class Server& srv)
{
	P(this->_location, "location respond");
	P(sizeof(*this->_location), "size location respond");
	debug();
	if (this->_httpver != "HTTP/1.1")
		return (errorPage("HTTP/1.1 505 HTTP Version not supported\n"));
	else if (!this->_location->getRedirection().empty())
		return ("HTTP/1.1 301 Moved Permanently\nLocation: " + this->_location->getRedirection() + "\n");
	else if ((this->_type == "GET" && !this->_location->getGetMethod()) ||
			(this->_type == "POST" && !this->_location->getPostMethod()) ||
			(this->_type == "DELETE" && !this->_location->getDeleteMethod()))
		return (errorPage("HTTP/1.1 405 Method Not Allowed\n"));
	else if (this->_type == "GET")
		return (GETRequest(srv));
	else if (this->_type == "POST")
		return (POSTRequest(srv));
	else if (this->_type == "DELETE")
		return (DELETERequest());
	else
		return (errorPage("HTTP/1.1 501 Not Implemented\n"));
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////// GET request //////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Request::GETRequest(Server& srv)
{
	std::string response;

	parseAccept();
	//debug();
	response = autoIndex();
	if (response.empty())
		response = GETResponse(srv);
	debug();
	return (response);
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
	std::ifstream t(("." + this->_root + this->_path + this->_filename).c_str());
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
	return ("HTTP/1.1 200 OK\n" + contentType() + "Content-length: " + ntostring(body.size()) + "\n\n" + body);
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

std::string Request::autoIndex()//// bug avec index
{
	std::string header = "<!DOCTYPE html>\n<html>\n <head>\n  <title>Index of [LOCATION]</title>\n </head>\n <body>\n<h1>Index of [LOCATION]</h1>\n";
	std::string table = "  <table>\n   <tr><th valign=\"top\"><img src=\"/icons/blank.gif\" alt=\"[ICO]\"></th><th>Name</th><th>Last modified</th><th>Size</th></tr>\n   <tr><th colspan=\"4\"><hr></th></tr>\n";
	std::string autoindex;
	size_t		ret = 0;

	if (this->_path.empty() && this->_filename.empty() && !this->_location->getIndex().empty())
	{
		this->_filename = "/" + this->_location->getIndex();
		return (autoindex);
	}
	else if (this->_filename.empty() && this->_location->getAutoIndex())
	{
		DIR* dirp;
		dirp = opendir(("." + this->_root + this->_path).c_str());
		if (dirp == NULL)
		{
			return (errorPage("HTTP/1.1 404 Not Found\n"));
		}
		if (this->_path.empty())
		{
			while ((ret = header.find("[LOCATION]")) != std::string::npos)
			{
				header.erase(ret, 10);
				header.insert(ret, this->_location->getPath());
				autoindex = header;
			}
		}
		// else	 ///sub header
		// {

		// }
		autoindex += table;
		autoindex += directoryListing(dirp);
		autoindex += "   <tr><th colspan=\"4\"><hr></th></tr>\n</table>\n<address>WebServ, created by pmaldagu, gverhelp, namenega</address>\n</body></html>";
		closedir(dirp);
		return ("HTTP/1.1 200 OK\nContent-Type: text/html\nContent-length: " + ntostring(autoindex.size()) + "\n\n" + autoindex);
	}
	return (autoindex);
}

std::string Request::directoryListing(DIR* dirp)
{
	struct dirent* direntp;
	std::string file = "<tr><td valign=\"top\"><img src=\"[ICON]\"></td><td><a href=\"[PATH]\">[NAME]</a></td><td align=\"right\">[LASTMOD]  </td><td align=\"right\">  [SIZE] </td></tr>\n";
	std::string buffer;
	std::string listing;
	size_t		ret = 0;
	struct stat info;

	while ((direntp = readdir(dirp)) != NULL)
	{
		std::string name = direntp->d_name;
		if (!this->_path.empty() && name == "..")
			listing += previousPage();
		else if (name != "." && name != "..")
		{
			buffer = file;
			ret = buffer.find("[PATH]");
			buffer.erase(ret, 6);
			if (this->_location->getPath() != "/")
				buffer.insert(ret, this->_location->getPath() + "/" + name);
			else
			 	buffer.insert(ret, this->_location->getPath() + name);
			ret = buffer.find("[NAME]");
			buffer.erase(ret, 6);
			buffer.insert(ret, direntp->d_name);
			stat(("." + this->_root + this->_path + "/" + name).c_str(), &info);
			ret = buffer.find("[ICON]");
			buffer.erase(ret, 6);
			if (!S_ISREG(info.st_mode))
				buffer.insert(ret, "/icons/folder.gif");
			else if ((name.find(".html") != std::string::npos) || (name.find(".txt") != std::string::npos))
				buffer.insert(ret, "/icons/text.gif");
			else
				buffer.insert(ret, "/icons/unknown.gif");
			ret = buffer.find("[LASTMOD]");
			buffer.erase(ret, 9);
			buffer.insert(ret, formatLastMod(&info.st_mtim));	//info.st_mtimespec pour mac
			ret = buffer.find("[SIZE]");
			buffer.erase(ret, 6);
			if (!S_ISREG(info.st_mode))
				buffer.insert(ret, "-");
			else
				buffer.insert(ret, formatSize(ntostring(info.st_size)));

			listing += buffer;
		}
	}
	return (listing);
}

std::string Request::formatSize(std::string size)
{
	std::string format;

	format = size;
	if (size.size() > 9)
	{
		if (size.size() == 10)
			format = size.substr(0, 1) + "." + size.substr(1, 1) + "G";
		else
			format = size.substr(0, size.size() - 3) + "G";
	}
	else if (size.size() > 6)
	{
		if (size.size() == 7)
			format = size.substr(0, 1) + "." + size.substr(1, 1) + "M";
		else
			format = size.substr(0, size.size() - 3) + "M";
	}
	else if (size.size() > 3)
	{
		if (size.size() == 4)
			format = size.substr(0, 1) + "." + size.substr(1, 1) + "K";
		else
			format = size.substr(0, size.size() - 3) + "K";
	}
	return (format);
}

std::string Request::formatLastMod(struct timespec* lastmod)
{
	char buffer[51];
	std::string format;

	memset(buffer, 0 , 51);
	struct tm * ptm = gmtime( &lastmod->tv_sec);
	ptm->tm_hour += 1;
	strftime( buffer, 50, "%d/%m/%y %R", ptm);
	format = buffer;
	return (format);
}

std::string Request::previousPage() ///bug avec default location
{
	std::string file = "<tr><td valign=\"top\"><img src=\"/icons/back.gif\"></td><td><a href=\"[PREV]\">Parent Directory</a></td><td>&nbsp;</td><td align=\"right\">  - </td></tr>";
	size_t ret;
	size_t past;
	
	ret = file.find("[PREV]");
	file.erase(ret, 6);
	past = this->_path.rfind("/");
	if (past == 0)
		file.insert(ret, this->_location->getPath());
	else
		file.insert(ret, this->_location->getPath() + this->_path.substr(0, this->_path.rfind("/") - 1));
	return (file);
}
	
///////////////////////////////////////////////////////////////////////////
//////////////////////////// POST request /////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Request::POSTRequest(Server& srv)
{
	(void)srv;
	std::list<std::string>::iterator 	it = this->_buffer.begin();
	size_t								size = 0;
	std::stringstream ss;

	for (; it != this->_buffer.end(); it++)
	{
		if ((*it).find("Content-Length: ") != std::string::npos)
			break;
	}
	if (it != this->_buffer.end())
	{
		ss << (*it).substr(16, (*it).size() - 1);
		ss >> size;
	}
	if (size > srv.getCmaxsize())
		return ("HTTP/1.1 413 Request Entity Too Large\n");
	parseBody();
	P(this->_body, "this body");
	return (postAppend());
}

std::string Request::postAppend()
{
	std::ofstream file_out;

	if (!this->_filename.empty())
	{
		file_out.open(("." + this->_location->getRoot() + this->_path + this->_filename).c_str(), std::ios_base::app);
		if (file_out.is_open())
		{
			file_out << this->_body;
			file_out.close();
			return ("HTTP/1.1 200 OK\n");
		}
	}
	return ("HTTP/1.1 404 Not Found\n");
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////// DELETE request ////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Request::DELETERequest()
{
	struct stat fileInfo;
	stat(("." + this->_root + this->_path + this->_filename).c_str(), &fileInfo);
	if (S_ISREG(fileInfo.st_mode))
		std::remove(("." + this->_root + this->_path + this->_filename).c_str());
	else
		return ("HTTP/1.1 202 Accepted\n"); 			/// maybe 404
	return ("HTTP/1.1 200 OK\n\n{\"success\":\"true\"}\n");
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////// Debug ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Request::debug( void )
{
	std::list<std::string>::iterator it;

	std::cout << RED << "TYPE : " << RESET << this->getType() << std::endl;
	std::cout << RED << "PATH : " << RESET << this->getPath() << std::endl;
	std::cout << RED << "HTTP : " << RESET <<this->getHttpver() << std::endl;
	std::cout << RED << "FILE NAME : " << RESET << this->getFilename() << std::endl;
	std::cout << RED << "ROOT : " << RESET << this->getRoot() << std::endl;
	std::cout << RED << "ACCEPT : " << RESET << std::endl;
	std::cout << RED << "LOCATION : " << RESET << this->_location->getPath() << std::endl;
	for (it = this->_accept.begin(); it != this->_accept.end(); it++)
		std::cout << "   " << (*it) << std::endl;
	std::cout << RED << "BUFFER : " << RESET << std::endl;
	for (it = this->_buffer.begin(); it != this->_buffer.end(); it++)
		std::cout << "   " << (*it) << std::endl;
}
