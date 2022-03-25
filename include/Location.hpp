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

        /*Setters*/
        void        setIndex(std::string index);
        void        setPath(std::string path, class Location *loca);
        void        setUploadDir(std::string uploaddir);
        void        setRoot(std::string root);
        void        setRedirection(std::string redirection);
        void        setAutoIndex(std::string autoindex);
        void        setGetMethod(bool b);
        void        setPostMethod(bool b);
        void        setDeleteMethod(bool b);

        /*Getters*/
        std::string getIndex() const;
        std::string getPath() const;
        std::string getUploadDir() const;
        std::string getRoot() const;
        std::string getRedirection() const;
        bool        getAutoIndex() const;
        bool        getGetMethod() const;
        bool        getPostMethod() const;
        bool        getDeleteMethod() const;

    private :
        std::string _index;
        std::string _path;
        std::string _upload_dir;
        std::string _root;
        std::string _redirection;
        bool        _autoindex;
        bool        _get_method;
        bool        _post_method;
        bool        _delete_method;
};

#endif