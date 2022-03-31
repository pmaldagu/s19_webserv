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
# include <sys/stat.h>
# include <dirent.h>
# include <sys/types.h>
# include <ctime>
 
# include "Webserv.hpp"
# include "Config.hpp"
# include "Server.hpp"
# include "Location.hpp"
# include "Client.hpp"
# include "Request.hpp"
# include "CGI.hpp"
# include "utils.hpp"

# define BUFFERSIZE 4096

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#define P(x, name) std::cout << name << " : " << YELLOW << x << RESET << std::endl;

#endif
