#ifndef LIB_HPP
# define LIB_HPP

# include <iostream>
# include <unistd.h>
# include <cstdio>
# include <list>
# include <string>
# include <cstring>
# include <vector>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <exception>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <functional>
# include <arpa/inet.h>
# include <fcntl.h>
 
# include "Webserv.hpp"
# include "Config.hpp"
# include "Server.hpp"
# include "Location.hpp"
# include "Client.hpp"
# include "Request.hpp"
# include "CGI.hpp"

# define BUFFERSIZE 4096

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#endif
