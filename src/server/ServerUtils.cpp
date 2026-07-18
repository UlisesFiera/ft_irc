#include "server/Server.hpp"

int	Server::nick2fd(const std::string &nick)
{
	std::map<int, Client>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->second.getNick() == nick)
			return (it->first);
	}
	return (-1);
}

size_t	Server::findcrfl(const std::string &stream)
{
	return (stream.find("\r\n"));
}

const std::string	&Server::isMember(const std::string &nick)
{
	std::vector<std::string>			v;
	std::vector<std::string>::iterator	it = std::find(_channels.begin(), _channels.end(), nick);

	if (it != v.end())
		return (*it);
	else
		return ("");
}

void	Server::createStreamingResponse(const Client &client, const Message &message, const std::vector<std::string> &channel)
{
	for (size_t i = 0; i < channel.size(); i++)
	{	
		_event_manager.update(nick2fd(channel[i]), POLLOUT);
		client.setResponse(Response(client, message, nick2fd(channel[i])));
	}
}

void	Server::createStreamingResponse(const Client &client, const Message &message, const int &target)
{
	if (target != client.getNick())
		_event_manager.update(nick2fd(target), POLLOUT);
	client.setResponse(Response(client, message, nick2fd(target)));
}
