#include "server/Server.hpp"



void	Server::kickChannel(Client &client, const Message &message)
{
	if (message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}


}