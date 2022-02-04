#include "../include/lib.hpp"

Location::Location()
{
    this->_index = "";
    this->_path = "";
    this->_error_page = "";
    this->_upload_dir = "";
    this->_root = "";
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
        this->_error_page = copy._error_page;
        this->_upload_dir = copy._upload_dir;
        this->_root = copy._root;
        this->_get_method = copy._get_method;
        this->_post_method = copy._post_method;
        this->_delete_method = copy._delete_method;
    }
    return (*this);
}

void Location::setIndex(std::string index)
{
    index.erase(std::remove_if(index.begin(), index.end(), isspace), index.end());
    index.erase(0, 5);
    this->_index = index;
}

void Location::setPath(std::string path)
{
    path.erase(std::remove_if(path.begin(), path.end(), isspace), path.end());
    path.erase(0, 8);
    path.pop_back();
    this->_path = path;
}

void Location::setErrorPage(std::string errorpage)
{
    errorpage.erase(std::remove_if(errorpage.begin(), errorpage.end(), isspace), errorpage.end());
    errorpage.erase(0, 10);
    this->_error_page = errorpage;
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

std::string Location::getIndex() const
{
    return (this->_index);
}

std::string Location::getPath() const
{
    return (this->_path);
}

std::string Location::getErrorPage() const
{
    return (this->_error_page);
}

std::string Location::getUploadDir() const
{
    return (this->_upload_dir);
}

std::string Location::getRoot() const
{
    return (this->_root);
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