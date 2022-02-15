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

        Request&					operator=(Request const& copy);

		/*parser*/
		void						splitBuffer(char *buffer);
		void						parseHttpVersion();
		//void 						parse( void );
		void						parseType( void );
		void						parsePath( void );
		void						parseAccept( void );

		/*setter
		void						setType(std::string type);
		void						setPath(std::string path);
		void						setAccept(std::string accept);
		*/

		/*getter*/
		std::vector<std::string>	getBuffer( void ) const;
		std::string					getType( void ) const;
		std::string					getPath( void ) const;
		std::vector<std::string>	getAccept( void ) const;
		std::string					getHttpver( void ) const;
		std::string					getLocation( void ) const;
		std::string					getFilename( void ) const;

		/*respond*/
		std::string					returnFile(class Server& srv);
		std::string					createHeader(class Server& srv);
		std::string					respond(class Server& srv);
		
		/*check*/
		std::vector<class Location>::iterator checkPath(class Server &srv);
		bool						checkMethod(Location& location);
		std::string					checkStatus(class Server& srv);
		std::string					checkType(class Server& srv);
		std::string					checkContent(class Server &srv);

		/*debug*/
		void						debug( void );

    private :
		std::vector<std::string>	_buffer;
		std::string					_type;
		std::string					_path;
		std::vector<std::string>	_accept;
		std::string					_httpver;
		std::string					_filename;
		std::string					_location;
};

#endif
