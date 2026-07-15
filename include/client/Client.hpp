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
		time_t const	&getLastActivity() const;		

		// setters
		void 			setLastActivity();
		void			setPort(const int &port);
	private:
		int				_port;
		time_t 			_lastActivity;
};		

#endif
