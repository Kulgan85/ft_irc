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

	for (int i = 0; i < targets.size(); i++)
	{
		if (targets[i][0] == '#' || targets[i][0] == '&')
		{
			// TODO: Channel
		}
		else
		{
			std::map<int, Client*>::iterator it;
			for (it = _clients.begin(); it != _clients.end(); it++)
			{
				if ((*it).second->getNickname() == targets[i])
				{
					std::string toSend = ":";
					toSend.append(_clients.find(sender_fd)->second->getNickname());
					toSend.append(" PRIVMSG ");
					toSend.append((*it).second->getNickname());
					toSend.append(args[2]);
					toSend.append("\r\n");
					send((*it).second->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
					break;
				}
			}
			if (it == _clients.end())
			{
				std::string toSend = ":ircserv 401 ";
				toSend.append(_clients.find(sender_fd)->second->getNickname());
				toSend.append(" :No such nick\r\n");
				send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			}
		}
	}
}