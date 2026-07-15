#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <vector>

class Message
{
	public:
		Message(std::string raw_message);
		~Message();

		std::string					getCommand() const;
		std::vector<std::string>	getParams() const;
		std::string					getTrailing() const;

	private:
		std::string					_command;
		std::vector<std::string>	_params;
		std::string					_trailing;
};

#endif