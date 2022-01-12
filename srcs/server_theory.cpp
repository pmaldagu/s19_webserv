/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/11 13:02:11 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/12 13:41:24 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* REFERENCE
 * https://www.ibm.com/docs/en/i/7.4?topic=designs-using-poll-instead-select
 * https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
 * https://reqbin.com/req/c-1n4ljxb9/curl-get-request-example
 * https://drive.google.com/file/d/0B3msld7qnNOhN1NXaFIwSFU2Mjg/view?resourcekey=0-ngY0neP78dxJKlFv0PJoDQ
 * https://stackoverflow.com/questions/32596553/sockaddr-structure-sys-socket-h
 * https://linuxize.com/post/curl-post-request/
 * https://www.geeksforgeeks.org/socket-programming-cc/
 * https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
 * https://stackoverflow.com/questions/970979/what-are-the-differences-between-poll-and-select
 * https://www.linuxtoday.com/blog/multiplexed-i0-with-poll/
 */

#include <sys/socket.h>
#include <cstring>
#include <cstdio>

int main()
{
	/* 1. Creation de socket
	 *
	 * int server_fd = socket(domain, type, protocol);
	 *
	 * domain (address family) : AF_NET (IP), AF_UNIX (local channel, similar to pipes)
	 * type (type of service) : SOCK_STREAM (virtual circuit service)
	 * protocol : no variation of the protocol for virtual circuit service donc 0
	 */

	int server_fd;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("cannot create socket");
		return 0;
	}

	/* 2. Indentify (name) a socket
	 *
	 * assigning a transport address to the socket (a port number in IP networking)
	 * In sockets the operation is called bind
	 *
	 * int bind(int socket, const struct sockaddr *address, socklen_t address_len)
	 *
	 * socket: socket create with socket system call
	 * sockaddr: struct info for specific com type, we use sockddre_in for ip networking
	 * struct sockaddr_in
	 * {
	 * 		__uint8_t         sin_len;
	 * 		sa_family_t       sin_family;
	 * 		in_port_t         sin_port;
	 * 		struct in_addr    sin_addr;
	 * 		char              sin_zero[8];
	 * };
	 * sin_family: address family in that case AF_INET (IPv4)
	 * sin_port: the port number
	 * sin_addr: my machine Ip address (host ?) 0.0.0.0 (let the os choose) (INADDR_ANY)
	 */
	
	 struct sockaddr_in address;
	 const int PORT = 8080; //where the clients cans reach at

	 /* htonl converts a long integer (e.g. address) to a network representation */

	 /* htons converts a short integer (e.g. port) to a network representation */

	 memset((char *)&address, 0, sizeof(address));
	 address.sin_family = AF_INET;
	 address.sin_addr.s_addr = htonl(INADDR_ANY);
	 address.sin_port = htons(PORT);

	 if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	 {
		 perror("bind failed");
		 return 0;
	 }

	 /* 3. On the server, wait for an incoming connection
	  *
	  * int listen(int socket, int backlog)
	  *
	  * backlog: defines the maximum number of pending connections that can be queued
	  * accept call: grads the first connection request on the queue of pending connection
	  */
}
	 
