#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "lib.hpp"

class Client
{
    public :
        Client();
        Client(Client const& copy);
        virtual ~Client();

        Client& operator=(Client const& copy);

    private :
        int     fd;
};

#endif