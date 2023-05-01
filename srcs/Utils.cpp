#include "../includes/Utils.hpp"

std::deque<std::string>	_splitMessages(std::string message)
{
	std::deque<std::string>	ret;

	while (message.find('\n') != std::string::npos)
	{
		ret.push_back(message.substr(0, message.find('\n') + 1));
		message.erase(0, message.find('\n') + 1);
	}
	return (ret);
}

std::vector<std::string> _splitString(std::string str)
{
	if (str.at(str.length() - 1) == '\n')
		str.erase(str.length() - 1);
	if (str.at(str.length() - 1) == '\r')
		str.erase(str.length() - 1);
	std::vector<std::string> tokens;
	std::string	buf;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		buf.clear();
		if (str[i] == ':')
		{
			// This next line removes the ':' character from the start of the last parameter
			++i;
			while (i < str.size())
			{
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
		else if (str[i] != ' ')
		{
			while (str[i] != ' ' && i < str.size())
			{
				if (!(_isValidChar(str[i])))
				{
					if (i == 0 && str[i] == '*' && tokens.size() == 2 && tokens[0].compare("USER") == 0)
						;
					else
					{
						tokens.clear();
						return (tokens);
					}
				}
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
	}
	return (tokens);
}

bool	_isValidChar(char c)
{
	if (c == '\0' || c == '\r' || c == '\n' || c == ':' || c == ' ')
		return (false);
	return (true);
}

std::vector<std::string>	getTargets(std::string commaList)
{
	std::vector<std::string> output;
	std::string::size_type prevPos = 0, pos = 0;

	while ((pos = commaList.find(',', pos)) != std::string::npos)
	{
		std::string subStr(commaList.substr(prevPos, pos - prevPos));
		output.push_back(subStr);
		prevPos = ++pos;
	}
	output.push_back(commaList.substr(prevPos, pos - prevPos));
	return output;
}