#include "server/Server.hpp"

void	Server::changeNick(Client &client, const Message &message)
{
	if (message.getParams().size() != 1)
	{
		client.setResponse(Response(client, message, ERR_NONICKNAMEGIVEN));
		return ;
	}
	if (message.getParams()[0] == client.getNick())
		return ;
	if (registerNick(client, message))
		createStreamingResponse(client, message, nick2fd(client.getNick()));
}
