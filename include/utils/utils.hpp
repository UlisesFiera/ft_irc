#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include "message/Message.hpp"

void		printcrlf(const std::string &str, size_t bytes);
std::string	getCommandString(const commands &command);

#endif
