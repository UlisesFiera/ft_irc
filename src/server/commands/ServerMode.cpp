#include "server/Server.hpp"

bool Server::modeChecker(std::vector<std::string> &params)
{
    if (params.size() < 2)
        return false;

    int pass_arg = 0;
    int limit_arg = 0;
    int operator_arg = 0;
    int mode_blocks = 0;

    for (size_t i = 1; i < params.size(); i++)
    {
        if (params[i].empty() || (params[i][0] != '+' && params[i][0] != '-'))
            break;
            
        mode_blocks++;
        char sign = params[i][0];

        for (size_t j = 1; j < params[i].size(); j++)
        {
            char c = params[i][j];

            if (c == '+' || c == '-')
            {
                sign = c;
                continue;
            }

            if (c != 'i' && c != 't' && c != 'k' && c != 'l' && c != 'o')
                return false; 

            if (c == 'k')
                pass_arg++;
            else if (c == 'l')
            {
                if (sign == '+')
                    limit_arg++;
            }
            else if (c == 'o')
                operator_arg++;
        }
    }

    size_t expected_total_params = 1 + mode_blocks + pass_arg + limit_arg + operator_arg;

    if (params.size() != expected_total_params)
        return false;

    return true;
}

void Server::executeMode(std::vector<std::string> &params, Client &client, const Message &message)
{
    std::string channel_name = params[0];
    
    size_t arg_idx = 1;
    while (arg_idx < params.size() && !params[arg_idx].empty() && 
          (params[arg_idx][0] == '+' || params[arg_idx][0] == '-'))
    {
        arg_idx++;
    }

    for (size_t i = 1; i < arg_idx; i++)
    {
        char sign = params[i][0];

        for (size_t j = 1; j < params[i].size(); j++)
        {
            char c = params[i][j];

            if (c == '+' || c == '-')
            {
                sign = c;
                continue;
            }

            if (c == 'i')
			{
				if (sign == '+')
                	_channels[channel_name]->setInviteOnly(true);
				if (sign == '-')
                	_channels[channel_name]->setInviteOnly(false);
			}
            else if (c == 't')
                _channels[channel_name]->setTopicRestricted((sign == '+'));
            
            else if (c == 'k')
            {
                std::string pass = params[arg_idx++];
                if (sign == '+')
                    _channels[channel_name]->setPassword(pass);
                else
                    _channels[channel_name]->setPassword("");
            }
            else if (c == 'l')
            {
                if (sign == '+')
                {
                    size_t limit = atoi(params[arg_idx++].c_str());
                    _channels[channel_name]->setUserLimit(limit);
                }
                else
                    _channels[channel_name]->setUserLimit(0);
            }
            else if (c == 'o')
            {
                std::string target_nick = params[arg_idx++];
                if (sign == '+')
				{
					if (_channels[channel_name]->isInChannel(target_nick))
                    	_channels[channel_name]->setOperator(target_nick);
					else
					{
						client.setResponse(Response(client, message, ERR_NOTONCHANNEL));
						return;
					}
				}
                else
                    _channels[channel_name]->removeOperator(target_nick); 
            }
        }
    }
    
	createStreamingResponse(client, message, _channels[channel_name]->getNicks());
	createStreamingResponse(client, message, client.getFd());
}

void Server::channelMode(Client &client, const Message &message)
{
    if (message.getParams().empty())
    {
        client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
        return;
    }

    std::string channel_name = message.getParams()[0];
    std::vector<std::string> params = message.getParams();

	// check included to manage the mandatory connection check MODE <nick> +i
	if (channel_name[0] != '#' && params.size() == 2 && params[1] == "+i")
	{
		createStreamingResponse(client, message, client.getFd());
		return ;
	}
    if (!checkChannel(channel_name))
    {
        client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
        return;
    }

    if (params.size() == 1)
    {
		client.setResponse(Response(client, message, RPL_CHANNELMODEIS));
        return;
    }

    if (!_channels[channel_name]->isOperator(client.getNick()))
    {
        client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
        return;
    }

    if (!modeChecker(params))
    {
        client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
        return;
    }

    executeMode(params, client, message);
}