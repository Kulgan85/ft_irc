#include "../includes/Server.hpp"

void	Server::NOTICE(const int &sender_fd)
{
	_sendMessage(sender_fd, true);
}

void	Server::PMSG(const int &sender_fd)
{
	_sendMessage(sender_fd);
}

void	Server::_sendMessage(int sender_fd, bool silent)
{
	std::vector<std::string>	args = _splitString(this->_clients[sender_fd]->getMessage());
	Client*	c = _clients.find(sender_fd)->second;

	if (!c->getIsRegistered())
	{
		if (silent)
			return;
		std::string toSend = ":ircserv 451 ";
		toSend.append(c->getNickname());
		toSend.append(" :You have not registered\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	if (args.size() == 2)
		args.push_back("");
	if (args.size() >= 2)
	{
		std::vector<std::string>	targets = getTargets(args[1]);
		for (std::vector<std::string>::size_type i = 0; i < targets.size(); i++)
		{
			if (targets[i][0] == '#' || targets[i][0] == '&')
			{
				std::map<std::string, Channel*>::iterator it = _channels.find(targets[i]);
				if (it != _channels.end())
				{
					it->second->SendMessage(c, args[2]);
				}
				else if (!silent)
				{
					std::string toSend = ":ircserv 403 ";
					toSend.append(c->getNickname());
					toSend.push_back(' ');
					toSend.append(targets[i]);
					toSend.append(" :No such channel\r\n");
					send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
				}
			}
			else
			{
				std::map<int, Client*>::iterator it;
				for (it = _clients.begin(); it != _clients.end(); it++)
				{
					if ((*it).second->getNickname() == targets[i])
					{
						std::string toSend = ":";
						toSend.append(c->getNickname());
						if (silent)
							toSend.append(" NOTICE ");
						else
							toSend.append(" PRIVMSG ");
						toSend.append((*it).second->getNickname());
						toSend.append(" :");
						toSend.append(args[2]);
						toSend.append("\r\n");
						send((*it).second->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
						break;
					}
				}
				if (!silent && it == _clients.end())
				{
					std::string toSend = ":ircserv 401 ";
					toSend.append(_clients.find(sender_fd)->second->getNickname());
					toSend.append(" :No such nick\r\n");
					send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
				}
			}
		}
	}
	else
	{
		std::string toSend = ":ircserv 411 ";
		toSend.append(c->getNickname());
		toSend.append(" :No recipient given (");
		toSend.append(args[0]);
		toSend.append(")\r\n");
		send(sender_fd, toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return ;
	}
}