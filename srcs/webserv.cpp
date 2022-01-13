#include "../include/webserv.hpp"
#include "../include/parsing.hpp"

int main(int argc, char **argv)
{
    std::list<class Server> servList;

    if (argc != 2)
    {
        std::cerr << RED << "Error : Configuration file is missing." << RESET << std::endl;
        return (-1);
    }
    servList = parsingConf(argv[1]);
    return (0);
}