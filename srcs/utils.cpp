#include "../include/lib.hpp"

std::string ntostring(size_t Number)
{
    std::ostringstream convert;

    convert << Number;
    return (convert.str());
}