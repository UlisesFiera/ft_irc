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

    bool seen_i = false;
    bool seen_t = false;
    bool seen_k = false;
    bool seen_l = false;

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
                if (!seen_i)
                {
                    _channels[channel_name]->setInviteOnly(sign == '+');
                    seen_i = true;
                }
            }
            else if (c == 't')
            {
                if (!seen_t)
                {
                    _channels[channel_name]->setTopicRestricted(sign == '+');
                    seen_t = true;
                }
            }
            else if (c == 'k')
            {
                std::string pass = params[arg_idx++]; 
                if (!seen_k)
                {
                    if (sign == '+')
                        _channels[channel_name]->setPassword(pass);
                    else
                        _channels[channel_name]->setPassword("");
                    seen_k = true;
                }
            }
            else if (c == 'l')
            {
                if (sign == '+')
                {
                    size_t limit = atoi(params[arg_idx++].c_str()); 
                    if (!seen_l)
                    {
                        _channels[channel_name]->setUserLimit(limit);
                        seen_l = true;
                    }
                }
                else
                {
                    if (!seen_l)
                    {
                        _channels[channel_name]->setUserLimit(0);
                        seen_l = true;
                    }
                }
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

    if (!checkChannel(channel_name))
    {
        client.setResponse(Response(client, message, ERR_NOSUCHCHANNEL));
        return;
    }

    if (channel_name[0] != '#' && params.size() == 2 && params[1] == "+i")
    {
        createStreamingResponse(client, message, client.getFd());
        return ;
    }

    if (params.size() >= 2 && !_channels[channel_name]->isOperator(client.getNick()))
    {
        client.setResponse(Response(client, message, ERR_CHANOPRIVSNEEDED));
        return;
    }

    if (params.size() >= 2 && !modeChecker(params))
    {
        client.setResponse(Response(client, message, ERR_NEEDMOREPARAMS));
        return;
    }

    if (params.size() == 1)
    {
        std::string active_modes = "";

        if (_channels[channel_name]->getInviteOnly() || _channels[channel_name]->getTopicRestricted() || _channels[channel_name]->getPassword() != "" || _channels[channel_name]->getUserLimit() > 0)
            active_modes += "+";

        if (_channels[channel_name]->getInviteOnly()) active_modes += "i";
        if (_channels[channel_name]->getTopicRestricted()) active_modes += "t";
        if (_channels[channel_name]->getPassword() != "") active_modes += "k";
        if (_channels[channel_name]->getUserLimit() > 0) active_modes += "l";

        _channels[channel_name]->setChannelModes(active_modes);

        std::cout << _channels[channel_name]->getChannelModes() << std::endl;

		client.setResponse(Response(client, message, RPL_CHANNELMODEIS));
        return;
    }

    executeMode(params, client, message);
}