/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/25 15:03:01 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/25 15:21:27 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class Webserv
{
	public:
		Webserv();
		Webserv(Webserv const& copy);
		~Webserv();

		Webserv& operator=(Webserv const& copy);

		/*Socket management*/
		void createSocket( void );
		void setNonblocking( void );
		void bindSocket( void );
		void setListen( void );
		void initializePoll( void );
		void launch( void );

	private:
		std::list<Server> _servers;
		struct pollfd* _fds;

};

