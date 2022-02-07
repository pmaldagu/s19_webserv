#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "lib.hpp"

class Request
{
    public :
        Request();
        Request(char* buffer);
        Request(Request const& copy);
        virtual ~Request();

        Request& operator=(Request const& copy);

		/*parser*/
		void splitBuffer(char *buffer);
		//void parse( void );
		void parseType( void );
		void parsePath( void );
		void parseAccept( void );

		/*setter
		void setType(std::string type);
		void setPath(std::string path);
		void setAccept(std::string accept);
		*/

		/*getter*/
		std::vector<std::string> getBuffer( void ) const;
		std::string getType( void ) const;
		std::string getPath( void ) const;
		std::vector<std::string> getAccept( void ) const;

		/*respond*/
		std::string respond(class Server& srv);
		
		/*debug*/
		void debug( void );
			

    private :
		std::vector<std::string> _buffer;
		std::string	_type;
		std::string _path;
		std::vector<std::string> _accept;
		
};

#endif
