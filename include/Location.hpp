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
        void        setPath(std::string path);
        void        setErrorPage(std::string errorpage);
        void        setUploadDir(std::string uploaddir);
        void        setRoot(std::string root);
        void        setGetMethod(bool b);
        void        setPostMethod(bool b);
        void        setDeleteMethod(bool b);

        std::string getIndex() const;
        std::string getPath() const;
        std::string getErrorPage() const;
        std::string getUploadDir() const;
        std::string getRoot() const;
        bool        getGetMethod() const;
        bool        getPostMethod() const;
        bool        getDeleteMethod() const;

    private :
        std::string _index;
        std::string _path;
        std::string _error_page;
        std::string _upload_dir;
        std::string _root;
        bool        _get_method;
        bool        _post_method;
        bool        _delete_method;
};

#endif