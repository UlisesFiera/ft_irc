#include "server/Server.hpp"

void	Server::pingpong(Client &client, const Message &message)
{
	if (message.getCommand() == PING)
		createStreamingResponse(client, message, client.getFd());
	else
		;
}
