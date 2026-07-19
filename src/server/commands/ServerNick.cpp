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
	{
		std::cout << "Nick changed successfully" << std::endl;
		createStreamingResponse(client, message, client.getFd());
	}
}
