#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "server.hpp"

class Location : public Server
{
    public :
        Location();
        Location(Location const& copy);
        ~Location();

        Location& operator=(Location const& copy);
};

#endif