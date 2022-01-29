#include "../include/webserv.hpp"
#include "../include/config.hpp"
#include "../include/server.hpp"
#include "../include/location.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << RED << "Error : Configuration file is missing." << RESET << std::endl;
        return (-1);
    }
    try
    {
        Config *file = new Config(argv[1]);

        file->setServ();
        ///////////////// Print my values ///////////////////// 
        std::cout << file->getServ()[0].getRoot() << std::endl;
        std::cout << file->getServ()[0].getPort() << std::endl;
        std::cout << file->getServ()[0].getHost() << std::endl;
        std::cout << file->getServ()[1].getPort() << std::endl;
        ///////////////////////////////////////////////////////

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