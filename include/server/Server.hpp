#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <map>
# include <arpa/inet.h>

# include "server/Socket.hpp"
# include "server/EventManager.hpp"
# include "client/Client.hpp"

# define BUFFER_SIZE 1024

class Server
{
	public:
		Server();
		Server(const Server &other);
		Server &operator=(const Server &other);
		~Server();

		// Functions
		void					run(const int &port, const std::string &password);

	private:
		// Variables
		
		int						_port;
		std::string				_password;
		Socket					_listening_socket;
		EventManager			_event_manager;
		std::map<int, Client>	_clients;
		std::vector<int>		_clients2rm;

		// Functions
		void					initListeningSocket();
		static void				signal_handler(int signum);
		void					acceptClients();
		void					readClients();
		std::string				readStream(int client_fd);
		void					removeClients();
		size_t					findcrfl(const std::string &stream);
};

#endif