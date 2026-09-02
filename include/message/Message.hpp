#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>
# include <iostream>

enum	commands
{
	INVALID,
	NONE,
	CAP,
	NICK,
	PASS,
	USER,
	JOIN,
	PRIVMSG,
	KICK,
	INVITE,
	TOPIC,
	MODE,
	PART,
	PING,
	PONG
};

class Message
{
	public:
		Message();
		Message(std::string stream);
		Message(const Message &copyMessage);
		Message &operator=(const Message &copyMessage);
		~Message();

		commands					getCommand() const;
		std::vector<std::string>	getParams() const;
		std::string					getTrailing() const;

		void						parse();
		
	private:
		void						parseMessage();
		commands 					resolveCommand();

		std::string					_stream;
		std::string					_command;
		std::vector<std::string>	_params;
		std::string					_trailing;
		commands					_cmd;
};

#endif
