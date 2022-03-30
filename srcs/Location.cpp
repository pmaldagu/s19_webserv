#include "../include/lib.hpp"

Location::Location()
{
    this->_index = "";
    this->_path = "";
    this->_upload_dir = "";
    this->_root = "";
    this->_redirection = "";
    this->_autoindex = true;
    this->_get_method = false;
    this->_post_method = false;
    this->_delete_method = false;
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
        this->_path = copy._path;
        this->_upload_dir = copy._upload_dir;
        this->_root = copy._root;
        this->_get_method = copy._get_method;
        this->_post_method = copy._post_method;
        this->_delete_method = copy._delete_method;
        this->_redirection = copy._redirection;
        this->_autoindex = copy._autoindex;
    }
    return (*this);
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////////// Setters //////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Location::setIndex(std::string index)
{
    index.erase(std::remove_if(index.begin(), index.end(), isspace), index.end());
    index.erase(0, 5);
    this->_index = index;
}

void Location::setPath(std::string path, class Location *loca)
{
    path.erase(std::remove_if(path.begin(), path.end(), isspace), path.end());
    if ((path.find("location/") == std::string::npos) || (path[path.size() - 1] != '{'))
    {
        delete loca;
        throw std::runtime_error("(.conf parsing Location): path parsing failed.");
    }
    path.erase(0, 8);
    path.erase(path.size() - 1);
    this->_path = path;
}

void Location::setUploadDir(std::string uploaddir)
{
    uploaddir.erase(std::remove_if(uploaddir.begin(), uploaddir.end(), isspace), uploaddir.end());
    uploaddir.erase(0, 10);
    this->_upload_dir = uploaddir;
}

void Location::setRoot(std::string root)
{
    root.erase(std::remove_if(root.begin(), root.end(), isspace), root.end());
    root.erase(0, 4);
    this->_root = root;
}

void Location::setRedirection(std::string redirection)
{
    redirection.erase(std::remove_if(redirection.begin(), redirection.end(), isspace), redirection.end());
    redirection.erase(0, 6);
    this->_redirection = redirection;
}

void Location::setAutoIndex(std::string autoindex)
{
    autoindex.erase(std::remove_if(autoindex.begin(), autoindex.end(), isspace), autoindex.end());
    autoindex.erase(0, 9);
    if (autoindex == "on")
        this->_autoindex = true;
    else if (autoindex == "off")
        this->_autoindex = false;
}

void Location::setGetMethod(bool b)
{
    if (b == true)
        this->_get_method = true;
}

void Location::setPostMethod(bool b)
{
    if (b == true)
        this->_post_method = true;
}

void Location::setDeleteMethod(bool b)
{
    if (b == true)
        this->_delete_method = true;
}

///////////////////////////////////////////////////////////////////////////
/////////////////////////////// Getters ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
std::string Location::getIndex() const
{
    return (this->_index);
}

std::string Location::getPath() const
{
    return (this->_path);
}

std::string Location::getUploadDir() const
{
    return (this->_upload_dir);
}

std::string Location::getRoot() const
{
    return (this->_root);
}

std::string Location::getRedirection() const
{
    return (this->_redirection);
}

bool Location::getAutoIndex() const
{
    return (this->_autoindex);
}

bool Location::getGetMethod() const
{
    return (this->_get_method);
}

bool Location::getPostMethod() const
{
    return (this->_post_method);
}

bool Location::getDeleteMethod() const
{
    return (this->_delete_method);
}