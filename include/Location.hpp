#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "lib.hpp"

class Location
{
    public :
        Location();
        Location(Location const& copy);
        virtual ~Location();

        Location& operator=(Location const& copy);
};

#endif