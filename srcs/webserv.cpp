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
        //std::cout << file->getServ()[0].getRoot() << std::endl;
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