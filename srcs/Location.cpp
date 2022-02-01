#include "../include/lib.hpp"

/***********************************************************/
/***********************************************************/
/*****************    Location Class   *********************/
/***********************************************************/
/***********************************************************/

Location::Location()
{
    this->_index = "";
    this->_postlocation = "";
}

Location::Location(Location const & copy)
{
    *this = copy;
}

Location::~Location() {}

Location& Location::operator=(Location const& copy)
{
    if (this != &copy)
    {
        this->_index = copy._index;
        this->_postlocation = copy._postlocation;
    }
    return (*this);
}

void Location::setIndex(std::string index)
{
    index.erase(std::remove_if(index.begin(), index.end(), isspace), index.end());
    index.erase(0, 5);
    this->_index = index;
}

void Location::setPostlocation(std::string postlocation)
{
    postlocation.erase(std::remove_if(postlocation.begin(), postlocation.end(), isspace), postlocation.end());
    postlocation.erase(0, 8);
    postlocation.pop_back();
    this->_postlocation = postlocation;
}

std::string Location::getIndex() const
{
    return (this->_index);
}

std::string Location::getPostlocation() const
{
    return (this->_postlocation);
}