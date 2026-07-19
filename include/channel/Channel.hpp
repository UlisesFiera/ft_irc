#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

#include "client/Client.hpp"

class Client;

class Channel
{
	public:
		Channel();
		Channel(const std::string &name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel();

		std::string					getName() const;
		std::vector<Client*>		getMembersVec() const;
		bool    					getInviteOnly() const;
		bool   						getTopicRestricted() const;
		std::string					getPassword() const;
		size_t						getUserLimit() const;
		std::vector<std::string>	getInvitedNicks() const;
		std::vector<std::string>	getOperators() const;

		void    					setName(std::string name);
		void    					setMembers(Client *members);
		void    					setInviteOnly(bool invite);
		void    					setTopicRestricted(bool restricted);
		void    					setPassword(std::string password);
		void    					setUserLimit(size_t limit);
		void						setInvitedNicks(std::vector<std::string> invitedNicks);
		void						setOperators(std::vector<std::string> operators);

		bool						checkName(const std::string &name) const;
		bool    					checkPassword(const std::string &password) const;
		bool						isInvited(const std::string &nick) const;
		bool						isOperator(const std::string &client_nick);
		std::vector<std::string>	getNicks();
		void						removeMembers(const Client &client_nick);

	private:
		std::string					_name;
		std::vector<Client*>		_members;
		bool                		_inviteOnly;
		bool                		_topicRestricted;
		std::string         		_password;
		size_t              		_userLimit;
		std::vector<std::string>	_invitedNicks;
		std::vector<std::string>	_operators;
};

#endif