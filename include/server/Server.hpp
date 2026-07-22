#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>
# include <arpa/inet.h>
# include <algorithm>

# include "events/Socket.hpp"
# include "events/EventManager.hpp"
# include "client/Client.hpp"
# include "message/Message.hpp"
# include "utils/utils.hpp"
# include "channel/Channel.hpp"

# define BUFFER_SIZE 1024
# define NICK_SIZE 30

class Server
{
	public:
		Server();
		Server(const Server &other);
		Server &operator=(const Server &other);
		~Server();

		// Functions
		void							run(const int &port, const std::string &password);

	private:
		// Variables
		
		int								_port;
		std::string						_host;
		std::string						_password;
		Socket							_listening_socket;
		EventManager					_event_manager;
		std::map<int, Client*>			_clients;
		std::vector<int>				_clients2rm;
		std::map<std::string, Channel*>	_channels;

		// Functions
		void							initListeningSocket();
		static void						signal_handler(int signum);
		void							acceptClients();
		void							readClients();
		std::string						readStream(int client_fd);
		void							registerClients();
		void							respondClients();
		void							respond(Response &response, int client_fd);
		void							checkWritingDone();
		void							removeClients();
		void							channelCleanup();
		void							removeFromChannels(Client &client);
		void							executeCommands();
		void							execute(Client &client, const Message &message, const commands &command);
		void							registerPass(Client &client, const Message &message);
		bool							registerNick(Client &client, const Message &message);
		void							registerUser(Client &client, const Message &message);

		// Commands
		void							joinChannel(Client &client, const Message &message);
		bool							checkChannel(const std::string &name);
		void							changeNick(Client &client, const Message &message);
		void							sendPrivmsg(Client &client, const Message &message);
		bool							nickSyntax(const std::string &nick);
		void							kickChannel(Client &client, const Message &message);
		void							channelTopic(Client &client, const Message &message);
		void							channelInvite(Client &client, const Message &message);
		void							channelMode(Client &client, const Message &message);
		bool							modeChecker(std::vector<std::string> &params);
		void							executeMode(std::vector<std::string> &params, Client &client, const Message &message);
		void							channelPart(Client &client, const Message &message);
		void							pingpong(Client &client, const Message &message);

		// Utils
		int								nick2fd(const std::string &nick);
		size_t							findcrfl(const std::string &stream);
		void							createStreamingResponse(Client &client, const Message &message, const std::vector<std::string> &channel);
		void							createStreamingResponse(Client &client, const Message &message, const int &target);
};

#endif