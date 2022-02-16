/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: namenega <namenega@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 16:43:23 by namenega          #+#    #+#             */
/*   Updated: 2022/02/16 15:55:11 by namenega         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/lib.hpp"

/* ************************* Constructors/Destructor ************************ */
/*	Default CGI Constructor */
CGI::CGI() {
		/*	Server variables */
		_SERVER_NAME = "";
		_SERVER_SOFTWARE = "";
		_GATEWAY_INTERFACE = "";
		/*	Request variables */
		_SERVER_PORT = "";
		_REQUEST_METHOD = "";
		_PATH_INFO = "";
		_PATH_TRANSLATED = "";
		_SCRIPT_NAME = "";
		_QUERY_STRING = "";
		_REMOTE_ADDR = "";
		_REMOTE_USER = "";
		/*	Others */
		_REDIRECT_STATUS = "";
		_BODY = "";
}

/*	Overload CGI Constructor */
// CGI::CGI(Request request, ParseConf parsing, Server server) {
CGI::CGI(Request & request, Server & server) {
		/*	Server variables */
		_SERVER_NAME = "";
		_SERVER_SOFTWARE = "";
		_GATEWAY_INTERFACE = "";
		/*	Request variables */
		_SERVER_PORT = "";
		_REQUEST_METHOD = "";
		_PATH_INFO = "";
		_PATH_TRANSLATED = "";
		_SCRIPT_NAME = "";
		_QUERY_STRING = "";
		_REMOTE_ADDR = "";
		_REMOTE_USER = "";
		/*	Others */
		_REDIRECT_STATUS = "";
		_BODY = "";

		setVariables(request, server);
		setEnv(&_env[0]);
}

CGI::CGI(const CGI & copy) {
	*this = copy;
}

/*	Destructor */
CGI::~CGI() {}

CGI &	CGI::operator=(const CGI & rhs) {
	if (this != &rhs) {
		this->_SERVER_NAME = rhs._SERVER_NAME;
		this->_SERVER_SOFTWARE = rhs._SERVER_SOFTWARE;
		this->_GATEWAY_INTERFACE = rhs._GATEWAY_INTERFACE;
		this->_SERVER_PORT = rhs._SERVER_PORT;
		this->_REQUEST_METHOD = rhs._REQUEST_METHOD;
		this->_PATH_INFO = rhs._PATH_INFO;
		this->_PATH_TRANSLATED = rhs._PATH_TRANSLATED;
		this->_SCRIPT_NAME = rhs._SCRIPT_NAME;
		this->_QUERY_STRING = rhs._QUERY_STRING;
		this->_REMOTE_ADDR = rhs._REMOTE_ADDR;
		this->_REMOTE_USER = rhs._REMOTE_USER;
		this->_REDIRECT_STATUS = rhs._REDIRECT_STATUS;
	}
	return (*this);
}

/* ******************************** Accessors ******************************* */

/*	Called everytime a new request is made */
void		CGI::setVariables(Request & request, Server & server) {
	char	currentDir[256];
	std::string	queryMark = request.getPath();

	getcwd(currentDir, 256);
	std::string	__path(currentDir);
	__path += "/www";

	_REQUEST_METHOD = request.getType();
	_PATH_INFO = request.getPath();
	_SERVER_PORT = server.getPort();
	_SERVER_SOFTWARE = server.getServername();
	_GATEWAY_INTERFACE = "CGI/1.1";
	// _SCRIPT_NAME = server.getCGIPath();
	_PATH_TRANSLATED = (char*)__path.c_str();
	if (queryMark.find('?') != std::string::npos) {
		_QUERY_STRING = queryMark.substr(queryMark.find('?') + 1);
		_PATH_INFO = queryMark.substr(0, queryMark.find('?'));
		_PATH_TRANSLATED += _PATH_INFO;
	}
	_SERVER_NAME = "http://localhost";
	/*	The request has succeeded. The info returned depends on the method
		(GET, POST) used in the request. */
	_REDIRECT_STATUS = "200";
}

void		CGI::setEnv(char **env) {
	std::string	s1 = "REQUEST_METHOD=";

	_env[0] = (char*)"SERVER_PROTOCOL=HTTP/1.1"; 
	_env[1] = strcat("REQUEST_METHOD=", _REQUEST_METHOD);
	_env[2] = strcat("PATH_INFO=", _PATH_INFO);
	_env[3] = strcat("SERVER_PORT=", _SERVER_PORT);
	_env[4] = strcat("SERVER_SOFTWARE=", _SERVER_SOFTWARE);
	_env[5] = strcat("GATEWAY_INTERFACE=", _GATEWAY_INTERFACE);
	_env[6] = strcat("SCRIPT_NAME=", _SCRIPT_NAME);
	_env[7] = strcat("PATH_TRANSLATED=", _PATH_TRANSLATED);
	_env[8] = strcat("QUERY_STRING=", _QUERY_STRING);
	_env[9] = strcat("REMOTE_ADDR=", _REMOTE_ADDR);
	_env[10] = strcat("REMOTE_USER=", _REMOTE_USER);
	_env[11] = strcat("SERVER_NAME=", _SERVER_NAME);
	_env[12] = strcat("REDIRECT_STATUS=", _REDIRECT_STATUS);
	_env[13] = NULL;
}

std::string	CGI::getBodyVar() const {
	return (_BODY);
}

/* ********************************* Others ********************************* */
/*	Classic strcat() for std::string conversion */
char*	CGI::strcat(std::string s1, std::string s2) {
	return (strdup((char*)(s1 + s2).c_str()));
}

/*	When client sends request (usually GET, POST method) to server, webserver
	makes a process to run the requested data. Then sen the result back.
	STEP1 : Make process by using fork(). Also open pipes.
	STEP2 : Execute CGI in child process.
	STEP3 : Pass requested data through pipes in Parent Process.
	CGI will give the result to webserver and it will make response based on
	this result.
	https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db */
int		CGI::execute(void) {
	int			fd[2], fdIn = dup(STDIN_FILENO), fdOut = dup(STDOUT_FILENO), ret = 1;
	char		buffer[BUFFERSIZE + 1];
	std::string	body = "";
	
	pipe(fd);
	if (!fork()) {
		dup2(fd[0], STDOUT_FILENO);
		dup2(fd[1], STDIN_FILENO);
		close(fd[0]);
		/*	execve:
			arg[0] = path of cgi program or cgi script
			arg[1] = path of cgi file
			arg[2] = NULL
			env = has parsed request and some more variables according to RFC3875
				[see: private attr.] */
		execve(_SCRIPT_NAME.c_str(), NULL, _env);
		// exit(0);
	}
	close(fd[1]);
	dup2(fd[0], 1);
	while (ret > 0) {
		memset(buffer, 0, BUFFERSIZE);
		ret = read(fd[0], buffer, BUFFERSIZE);
		body += buffer;
	}
	_BODY = body;
	dup2(fdIn, STDIN_FILENO);
	dup2(fdOut, STDOUT_FILENO);
	return (ret);
}
