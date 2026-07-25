#include "channel/Channel.hpp"

Channel::Channel () {}

Channel::Channel (const std::string &name) : _inviteOnly(false), _topicRestricted(false), _userLimit(0)
{
	if (checkName(name) == true)
		_name = name;
	else
		throw std::invalid_argument("[ERROR]: Invalid Channel name.");
}

Channel::Channel (const Channel &other)
{
	_name = other._name;
	_members = other._members;
	_inviteOnly = other._inviteOnly;
	_topicRestricted = other._topicRestricted;
	_password = other._password;
	_userLimit = other._userLimit;
	_invitedNicks = other._invitedNicks;
	_operators = other._operators;
}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_members = other._members;
		_inviteOnly = other._inviteOnly;
		_topicRestricted = other._topicRestricted;
		_password = other._password;
		_userLimit = other._userLimit;
		_invitedNicks = other._invitedNicks;
		_operators = other._operators;
	}
	return *this;
}

Channel::~Channel() {}


const std::string	&Channel::getName() const
{
	return _name;
}

const std::vector<Client*> &Channel::getMembersVec() const
{
	return _members;
}

const bool &Channel::getInviteOnly() const
{
	return _inviteOnly;
}

const bool &Channel::getTopicRestricted() const
{
	return _topicRestricted;
}

const std::string &Channel::getPassword() const
{
	return _password;
}

const size_t &Channel::getUserLimit() const
{
	return _userLimit;
}

const std::vector<std::string> &Channel::getInvitedNicks() const
{
	return _invitedNicks;
}

const std::vector<std::string> &Channel::getOperators() const
{
	return _operators;
}

const std::string	&Channel::getTopic() const
{
	return _topic;
}

const std::string	&Channel::getChannelModes() const
{
	return _channelModes;
}



void Channel::setName(const std::string &name)
{
	_name = name;
}

void Channel::setMembers(Client *members)
{
	_members.push_back(members);
}

void Channel::setInviteOnly(bool invite)
{
	_inviteOnly = invite;
}

void Channel::setTopicRestricted(bool restricted)
{
	_topicRestricted = restricted;
}

void Channel::setPassword(const std::string &password)
{
	_password = password;
}

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

void Channel::setInvitedNicks(const std::string &invitedNicks)
{
	_invitedNicks.push_back(invitedNicks);
}

void Channel::setOperators(const std::vector<std::string> &operators)
{
	_operators = operators;
}

void Channel::setOperator(const std::string &new_operator)
{
	_operators.push_back(new_operator);
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void Channel::setChannelModes(const std::string &modes)
{
	_channelModes = modes;
}




bool Channel::checkName(const std::string &name) const
{
	if (name.empty() || name[0] != '#')
        return false;
	for (size_t i = 1; i < name.size(); i++)
	{
		if ((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= '0' && name[i] <= '9') || name[i] == '_')
			continue;
		else
			return false;
	}
	return true;
}

bool Channel::checkPassword(const std::string &password) const
{
	if (_password == password)
		return true;
	else
		return false;
}

bool Channel::isInvited(const std::string &nick) const
{
	for (size_t i = 0; i < _invitedNicks.size(); i++)
	{
		if (this->getInvitedNicks()[i] == nick)
			return 1;
	}
	return 0;
}

void Channel::removeInvitation(const std::string &nick)
{
    std::vector<std::string>::iterator it;

    for (it = _invitedNicks.begin(); it != _invitedNicks.end(); ++it)
    {
        if (*it == nick)
        {
            _invitedNicks.erase(it);
            break;
        }
    }
}

bool Channel::isOperator(const std::string &client_nick)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client_nick)
			return true;
	}
	return false;
}

std::vector<std::string>	Channel::getNicks()
{
	std::vector<std::string> clients;

	for (size_t i = 0; i < this->_members.size(); i++)
	{
		std::string name = this->_members[i]->getNick();
		clients.push_back(name);
	}

	return clients;
}

void Channel::removeMember(const Client &client)
{
    std::vector<Client*>::iterator it;

    for (it = _members.begin(); it != _members.end(); ++it)
    {
		if ((*it)->getNick() == client.getNick())
        {
            _members.erase(it);
            break;
        }
    }
}

bool Channel::isInChannel(const std::string &client_nick)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i]->getNick() == client_nick)
			return true;
	}
	return false;
}

void Channel::removeOperator(const std::string &target_operator)
{
	std::vector<std::string>::iterator it;

	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (*it == target_operator)
		{
			_operators.erase(it);
			break;
		}
	}
}

bool Channel::isEmpty()
{
	if (this->getNicks().empty())
		return true;
	return false;
}

Client *	Channel::getClientFromTarget(const std::string &name)
{
	for (size_t i = 0; i < this->getMembersVec().size(); i++)
	{
		if (this->getMembersVec()[i]->getNick() == name)
			return this->getMembersVec()[i];
	}
	return NULL;
}