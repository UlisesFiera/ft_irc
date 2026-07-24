#include "server/Server.hpp"

void	Server::changeNick(Client &client, const Message &message)
{
	std::string	prev_nick = client.getNick();

	if (message.getParams().size() != 1)
	{
		client.setResponse(Response(client, message, ERR_NONICKNAMEGIVEN));
		return ;
	}
	if (message.getParams()[0] == client.getNick())
		return ;
	if (registerNick(client, message))
	{
		std::cout << "\033[35mircserv@asulgernan:\033[0mNick changed successfully from " << prev_nick << " to " << client.getNick() << std::endl;
		client.setOldNick(prev_nick);
		for (size_t i = 0; i < client.getChannels().size(); i++)
			createStreamingResponse(client, message, client.getChannels()[i]->getNicks());
		createStreamingResponse(client, message, client.getFd());
	}
}
