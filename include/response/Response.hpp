#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <vector>

# include "message/Message.hpp"
# include "client/Client.hpp"
# include "utils/utils.hpp"

enum ReplyCode
{
	// Welcome
	RPL_WELCOME            = 001,
	RPL_YOURHOST           = 002,
	RPL_CREATED            = 003,
	RPL_MYINFO             = 004,
	RPL_ISUPPORT		   = 005,

	// Command replies
	RPL_CHANNELMODEIS	   = 324,
	RPL_NOTOPIC            = 331,
	RPL_TOPIC              = 332,
	RPL_NAMREPLY           = 353,
	RPL_ENDOFNAMES         = 366,

	// Errors
	ERR_NOSUCHNICK         = 401,
	ERR_NOSUCHCHANNEL      = 403,
	ERR_CANNOTSENDTOCHAN   = 404,
	ERR_UNKNOWNCOMMAND     = 421,
	ERR_NONICKNAMEGIVEN    = 431,
	ERR_ERRONEUSNICKNAME   = 432,
	ERR_NICKNAMEINUSE      = 433,
	ERR_USERNOTINCHANNEL   = 441,
	ERR_NOTONCHANNEL       = 442,
	ERR_USERONCHANNEL      = 443,
	ERR_NOTREGISTERED      = 451,
	ERR_NEEDMOREPARAMS     = 461,
	ERR_ALREADYREGISTERED  = 462,
	ERR_PASSWDMISMATCH     = 464,
	ERR_CHANNELISFULL      = 471,
	ERR_UNKNOWNMODE        = 472,
	ERR_INVITEONLYCHAN     = 473,
	ERR_BADCHANNELKEY      = 475,
	ERR_CHANOPRIVSNEEDED   = 482
};

class Client;

class Response 
{
	public:
		// constructors
		Response();
		Response(const Client &client, const Message &message, const int &target);
		Response(const Client &client, const Message &message, const std::vector<int> &targets);
		Response(const Client &client, const Message &message, const ReplyCode &code);

		// copy constructors
		Response(const Response &copyResponse);

		// operator overrides
		Response &operator=(const Response &copyResponse);

		// destructors
		~Response();

		const std::string			&getResponse() const;
		const size_t				&getBytesSent() const;
		std::vector<int>			&getTargets();

		void						setBytesSent(const int &bytes);
	private:
		void						buildNumericResponse();
		void						buildStreamingResponse(const Message &message);
		std::string					getCommandString(const commands &command);
		std::string					header();

		std::string 				_addr;
		ReplyCode					_reply_code;
		commands					_command;
		std::string					_nick;
		std::string					_user;
		std::string					_message;
		std::string					_destination;
		std::string					_params;
		std::string					_response;
		size_t						_bytes_sent;
		std::string					_trailing;
		std::vector<int>			_targets;
		std::string					_old_nick;
};

#endif
