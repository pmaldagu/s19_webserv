#include "../includes/webserv.hpp"
#include "../includes/parsing.hpp"

int main(int argc, char **argv)
{
    std::list<class Server> servList;

    servList = parsingConf(argv[1]);
    return (0);
}