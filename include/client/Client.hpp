#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>

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
		time_t const		&getLastActivity() const;
		std::string const	&getStream() const;

		// setters
		void 				setLastActivity();
		void				setPort(const int &port);
		void				setStream(const std::string &stream);
	private:
		int					_port;
		time_t 				_lastActivity;
		std::string			_stream;
};		

#endif
