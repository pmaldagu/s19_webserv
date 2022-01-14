/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiplexer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmaldagu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 16:39:42 by pmaldagu          #+#    #+#             */
/*   Updated: 2022/01/14 14:53:42 by pmaldagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* Reference
 * https://programmerall.com/article/58761288145/
 * https://stackoverflow.com/questions/15560336/listen-to-multiple-ports-from-one-server/15560580
 * https://adrienblanc.com/projects/42/webserv
 * https://github.com/cclaude42/webserv
 * https://github.com/edisontim?tab=repositories
 * https://sites.google.com/site/rajboston1051/select/select-and-poll
 * https://developer.mozilla.org/fr/docs/Web/HTTP/Redirections
 * https://copyconstruct.medium.com/nonblocking-i-o-99948ad7c957
 * https://www.tecmint.com/limit-file-upload-size-in-nginx/
 * https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts
 * https://www.netsparker.com/blog/web-security/disable-directory-listing-web-servers/#:~:text=Directory%20listing%20is%20disabled%20by%20default%20on%20the%20Nginx%20configuration,disable%20the%20directory%20listing%20feature.
 * https://www.tutorialspoint.com/cplusplus/cpp_web_programming.html
 * https://sites.google.com/site/rajboston1051/select/select-and-poll
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

