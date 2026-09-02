#include "server/Server.hpp"

void	Server::pong(Client &client, const Message &message)
{
	if (message.getCommand() == PONG)
		createStreamingResponse(client, message, client.getFd());
}
