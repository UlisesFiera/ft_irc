#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include "message/Message.hpp"
# include <sstream>


void		printcrlf(const std::string &str, size_t bytes);
std::string	getCommandString(const commands &command);
std::string	to_string(const int &num);

#endif
