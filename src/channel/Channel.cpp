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


std::string	Channel::getName() const
{
	return _name;
}

std::vector<Client> Channel::getMembersVec() const
{
	return _members;
}

bool Channel::getInviteOnly() const
{
	return _inviteOnly;
}

bool Channel::getTopicRestricted() const
{
	return _topicRestricted;
}

std::string Channel::getPassword() const
{
	return _password;
}

size_t Channel::getUserLimit() const
{
	return _userLimit;
}

std::vector<std::string> Channel::getInvitedNicks() const
{
	return _invitedNicks;
}

std::vector<std::string> Channel::getOperators() const
{
	return _operators;
}

void Channel::setName(std::string name)
{
	_name = name;
}

void Channel::setMembers(Client members)
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

void Channel::setPassword(std::string password)
{
	_password = password;
}

void Channel::setUserLimit(size_t limit)
{
	_userLimit = limit;
}

void Channel::setInvitedNicks(std::vector<std::string> invitedNicks)
{
	_invitedNicks = invitedNicks;
}

void Channel::setOperators(std::vector<std::string> operators)
{
	_operators = operators;
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

bool Channel::isOperator(const std::string &client_nick)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i] == client_nick)
			return true;
	}
	return false;
}

std::vector<std::string>	Channel::getNicks() const
{
	std::vector<std::string> clients;

	for (size_t i = 0; i < this->_members.size(); i++)
	{
		std::string name = this->_members[i].getNick();
		clients.push_back(name);
	}

	return clients;
}