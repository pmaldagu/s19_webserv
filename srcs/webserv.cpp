#include "../include/webserv.hpp"
#include "../include/config.hpp"

int main(int argc, char **argv)
{
//    std::list<class Server> servList;

    if (argc != 2)
    {
        std::cerr << RED << "Error : Configuration file is missing." << RESET << std::endl;
        return (-1);
    }
    //servList = parsingConf(argv[1]);
    try
    {
        Config *file = new Config(argv[1]);
//        Webserv web(file);

//        web.launch();
    }
    catch (std::exception &e)
    {
//        web.close();
//        delete file;
    }
    return (0);
}