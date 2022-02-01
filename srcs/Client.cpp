#include "../include/lib.hpp"

Client::Client()
{

}

Client::Client(Client const& copy)
{
    *this = copy;
}

Client::~Client()
{

}

Client& Client::operator=(Client const& copy)
{
    if (this != &copy)
    {
        this->fd = copy.fd;
    }
    return (*this);
}