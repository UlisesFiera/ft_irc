#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>

class Channel
{
	public:
		Channel();
		Channel(const std::string &name);
		Channel(const Channel &other);
		Channel &operator=(const Channel &other);
		~Channel();

		std::string					getName() const;
		std::vector<std::string>	getMembers() const;
		bool    					getInviteOnly() const;
		bool   						getTopicRestricted() const;
		std::string					getPassword() const;
		size_t						getUserLimit() const;
		std::vector<int>			getInviteFDs() const;
		std::vector<int>			getOperatorFDs() const;

		void    					setName(std::string name);
		void    					setMembers(std::string members);
		void    					setInviteOnly(bool invite);
		void    					setTopicRestricted(bool restricted);
		void    					setPassword(std::string password);
		void    					setUserLimit(size_t limit);
		void						setInviteFDs(std::vector<int> inviteFDs);
		void						setOperatorFDs(std::vector<int> operatorFDs);

		bool						checkName(const std::string &name) const;
		bool    					checkPassword(const std::string &password) const;

	private:
		std::string					_name;
		std::vector<std::string>	_members;
		bool                		_inviteOnly;      // Modo +i
		bool                		_topicRestricted; // Modo +t
		std::string         		_password;        // Modo +k (vacío si no hay)
		size_t              		_userLimit;       // Modo +l (0 significa sin límite)
		std::vector<int>    		_invitedFDs;      // Lista blanca para el modo +i
		std::vector<int>    		_operatorFDs;     // Lista de administradores del canal (Modo +o)
};

#endif