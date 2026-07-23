#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <algorithm>

# include "message/Message.hpp"
# include "response/Response.hpp"
# include "channel/Channel.hpp"

class Response;

class Channel;

class Client 
{
	public:
		// constructors
		Client();

		// copy constructors
		Client(const Client &copyClient);

		// operator overrides
		Client &operator=(const Client &copyClient);

		// destructors
		~Client();

		// functions

		// getters
		time_t const					&getLastActivity() const;
		std::string const				&getStream() const;
		std::vector<Message>			&getMessages();
		bool const						&isRegistered() const;
		const std::string				&getPass() const;
		const std::string				&getNick() const;
		const std::string				&getUser() const;
		std::vector<Response>			&getResponses();
		const std::string				&getHost() const;
		std::vector<Channel*>			&getChannels();
		Channel							*getChannel(const std::string &channel);
		const int						&getFd() const;

		// setters
		void 							setLastActivity();
		void							setPort(const int &port);
		void							setStream(const std::string &stream);
		void							setMessage(const std::string &stream);
		void							setRegistered(const bool &status);
		void							setPass(const std::string &pass);
		void							setNick(const std::string &nick);
		void							setUser(const std::string &user);
		void							setName(const std::string &name);
		void							setResponse(const Response &response);
		void							setHost(const std::string &host);
		void							setChannel(Channel *channel);
		void							setFd(const int &fd);
		
		bool							isInChannel(const std::string &channel);
		void							removeChannel(const Channel &channel);

	private:
		int								_fd;
		std::string						_name;
		std::string						_host;
		int								_port;
		time_t 							_lastActivity;
		std::string						_stream;
		std::vector<Message>			_messages;
		bool							_registered;
		std::string						_pass;
		std::string						_nick;
		std::string						_user;
		std::vector<Response>			_responses;
		std::vector<Channel*>			_channels_joined;
};

#endif
