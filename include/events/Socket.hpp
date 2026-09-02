#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <cstring>
# include <cerrno>

class Socket
{
	public:
		Socket();
		Socket(const int &port);
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		~Socket();

		// setters and getters
		const int			&getSocketFd() const;
		const int		 	&getPort() const;
		const sockaddr_in 	&getAddress() const;
		const int 			&getIsListening() const;
	private:
		int 				_socketFd;
		int			 		_port;
		struct sockaddr_in 	_address;
		int 				_isListening;
		int					_opt;
};

#endif