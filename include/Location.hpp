#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "lib.hpp"

class Location
{
    public :
        Location();
        Location(Location const& copy);
        virtual ~Location();

        Location&   operator=(Location const& copy);

        void        setIndex(std::string index);
        void        setPostlocation(std::string postlocation);

        std::string getIndex() const;
        std::string getPostlocation() const;

    private :
        std::string _index;
        std::string _postlocation;
};

#endif