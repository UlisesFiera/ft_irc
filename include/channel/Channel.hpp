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

		const std::string					&getName() const;
		const std::vector<Client*>			&getMembersVec() const;
		const bool    						&getInviteOnly() const;
		const bool   						&getTopicRestricted() const;
		const std::string					&getPassword() const;
		const size_t						&getUserLimit() const;
		const std::vector<std::string>		&getInvitedNicks() const;
		const std::vector<std::string>		&getOperators() const;
		const std::string					&getTopic() const;
		const std::string					&getChannelModes() const;
		const std::string					&getKickReason() const;

		void    							setName(const std::string &name);
		void    							setMembers(Client *members);
		void    							setInviteOnly(bool invite);
		void    							setTopicRestricted(bool restricted);
		void    							setPassword(const std::string &password);
		void    							setUserLimit(size_t limit);
		void								setInvitedNicks(const std::string &invitedNicks);
		void								setOperators(const std::vector<std::string> &operators);
		void								setOperator(const std::string &new_operator);
		void								setTopic(const std::string &topic);
		void								setChannelModes(const std::string &modes);
		void								setKickReason(const std::string &modes);


		bool								checkName(const std::string &name) const;
		bool    							checkPassword(const std::string &password) const;
		bool								isInvited(const std::string &nick) const;
		void								removeInvitation(const std::string &nick);
		bool								isOperator(const std::string &client_nick);
		std::vector<std::string>			getNicks();
		void								removeMember(const Client &client_nick);
		bool 								isInChannel(const std::string &client_nick);
		void								removeOperator(const std::string &target_operator);
		bool								isEmpty();
		Client *							getClientFromTarget(const std::string &name);


	private:
		std::string							_name;
		std::vector<Client*>				_members;
		std::string         				_password;
		bool                				_inviteOnly;
		bool                				_topicRestricted;
		size_t              				_userLimit;
		std::vector<std::string>			_invitedNicks;
		std::vector<std::string>			_operators;
		std::string							_topic;
		std::string							_channelModes;
		std::string							_kickReason;
};

#endif