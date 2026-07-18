#include <iostream>

void	printcrlf(const std::string &str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '\r')
			std::cout << "\\r";
		else if (str[i] == '\n')
			std::cout << "\\n";
		else if (str[i] == '\t')
			std::cout << "\\t";
		else
			std::cout << str[i];
	}
	std::cout << std::endl;
}
