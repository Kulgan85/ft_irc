#include "../includes/Server.hpp"

static std::vector<std::string>	getTargets(std::string commaList)
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

void	Server::PMSG(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	std::vector<std::string>	targets = getTargets(args[1]);

	
}