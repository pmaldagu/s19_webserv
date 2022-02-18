/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: namenega <namenega@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 15:55:02 by namenega          #+#    #+#             */
/*   Updated: 2022/02/18 18:21:59 by namenega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "lib.hpp"

/*	CGI environnement variables are meta-variables given to a CGI program through the
	webserver. They give data about the client, its request and on the server.
	[see 'private:']
	
	Other request variables variables :
	- SERVER_PROTOCOL : protocol's name / revision in which the request has been
						made.
	- REMOTE_HOST : Name of the client's host.
	- AUTH_TYPE : Authentification's type used to protect the script (if needed).
	- REMOTE_IDENT : Client's distant username (if protected script).
	- CONTENT_TYPE : content type attached to the request if POST.
	- CONTENT_LENGTH : Content's length sent by the client.
	
	Other client variables :
	- HTTP_ACCEPT : Data accepted by the client
	- HTTP_ACCEPT_LANGUAGE : Language data accepted by the client
	- HTTP_USER_AGENT : Browser used by the client
	- HTTP_COOKIE : cookies, list of pair key=value;
	- HTTP_REFERER: absolute/part address of webpage from which the request has
					been sent. */

class	CGI
{
	public:
		/* ********************* Constructors/Destructor ******************** */
		CGI();
		// CGI(Request request, ParseConf parsing, Server server);
		// CGI(Request & request, Server & server);
		CGI(const CGI & copy);
		virtual ~CGI();
		CGI &	operator=(const CGI & rhs);

		/* **************************** Accessors *************************** */
		void		setVariables(Request & request, Server & server);
		void		setEnv(char **env);
		void		setRoot(std::string root);
		void		setExtension(std::string ext);

		std::string	getBodyVar() const;
		std::string getRoot() const;
		std::string	getExtension() const;

		/* ***************************** Others ***************************** */
		char *	strcat(std::string s1, std::string s2);
		int		execute(Request & request, Server & server);

	private:
		/*	Server variables */
		std::string	_SERVER_NAME;		/* Host name, DNS or IP address of the server. */
		std::string	_SERVER_SOFTWARE;	/* Name & version of the HTTP server.  */
		std::string	_GATEWAY_INTERFACE;	/* Revision de la specification CGI que le serveur use. */
		/*	Request variables */
		std::string	_SERVER_PORT;		/* Port's number on which the request has been sent. */
		std::string	_REQUEST_METHOD;	/* Method used to make the request (GET, POST, DELETE). */
		std::string	_PATH_INFO;			/* Script's path given by the client.
											URL: http://serveur.org/cgi-bin/monscript.cgi/marecherche
											PATH_INFO: marecherche */
		std::string	_PATH_TRANSLATED;	/* Path after conversion. */
		std::string	_SCRIPT_FILENAME;		/* Virtual path to the executed script. */
		std::string	_QUERY_STRING;		/* Holds everythin after '?' in the URL sent by the client.
											Every variables from a GET form are in QUERY_STRING
											(var1=val1&var2=...) */
		std::string	_REMOTE_ADDR;		/* Client's IP address. */
		std::string	_REMOTE_USER;		/* Client's username (if protected script). */
		/*	Others */
		std::string	_REDIRECT_STATUS;
		std::string	_BODY;
		std::string	_extType;
		char		*_env[14];
};

#endif
