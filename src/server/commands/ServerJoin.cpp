#include "server/Server.hpp"

bool Server::checkChannel(const std::string &name)
{
    if (_channels.find(name) != _channels.end())
        return true;
    return false;
}

void Server::joinChannel(Client &client, const Message &message)
{
	if (message.getParams().size() != 1 && message.getParams().size() != 2)
	{
		client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
		return;
	}

	std::string channel_name = message.getParams()[0];

	if (!(checkChannel(channel_name)))
	{
		try
		{
			Channel new_channel(channel_name);

			_channels[channel_name] = new_channel;
			_channels[channel_name].setMembers(client.getNick());
			std::cout << "[JOIN] Canal " << channel_name << " creado por " << client.getNick() << std::endl;
		}
        
        catch(const std::exception& e)
        {
            client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
            return;
        }
    }
	
	else
	{
		std::cout << "[JOIN] Client " << client.getNick() << " joined channel " << channel_name << std::endl;
		_channels[channel_name].setMembers(client.getNick());
	}
}