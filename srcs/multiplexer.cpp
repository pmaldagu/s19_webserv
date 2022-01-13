/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 16:39:42 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/13 17:31:44 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Reference
 * https://programmerall.com/article/58761288145/
 * https://stackoverflow.com/questions/15560336/listen-to-multiple-ports-from-one-server/15560580
 * https://adrienblanc.com/projects/42/webserv
 * https://github.com/cclaude42/webserv
 */

void multiplexerPoll(Webserv &webserv)
{
	int timeout;
	int reti = 0;

	timeout = (3 * 60 * 1000) //3 minute for example need change
	
	while (1)
	{
		ret = poll(webserv.getPollfd(), webserv.getServers().size(), timeout);
		if (rc =< 0)
			throw std::exception("poll() failed");

