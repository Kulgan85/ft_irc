#include "../includes/Channel.hpp"

Channel::Channel() : _name(""), _topic("")
{

}

Channel::Channel(std::string name) : _name(name), _topic("")
{

}

Channel::~Channel()
{
}

int	Channel::JoinChannel(Client* toJoin)
{
	std::string toSend;
	if (ClientIsInChannel(toJoin))
	{
		toSend = ":ircserv 443 ";
		toSend.append(toJoin->getNickname());
		toSend.push_back(' ');
		toSend.append(this->_name);
		toSend.append(" :is already on channel\r\n");
		send(toJoin->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return 1;
	}
	_clients.push_back(toJoin);
	toJoin->joined_channels.push_back(this);
	toSend = ":";
	toSend.append(toJoin->getNickname());
	toSend.append(" JOIN ");
	toSend.append(_name);
	toSend.append("\r\n");
	send(toJoin->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	if (!_topic.empty())
	{
		toSend = ":ircserv 332 :";
		toSend.append(_topic);
		toSend.append("\r\n");
		send(toJoin->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	}
	SendNames(toJoin);
	toSend = ":";
	toSend.append(toJoin->getNickname());
	toSend.append(" JOIN ");
	toSend.append(_name);
	toSend.append("\r\n");
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == toJoin)
			continue ;
		send(_clients[i]->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	}
	return 0;
}

void	Channel::SendNames(Client* sendTo)
{
	std::string toSend = ":ircserv 353 ";
	toSend.append(sendTo->getNickname());
	toSend.append(" @ ");
	toSend.append(_name);
	toSend.append(" :");
	for (size_t i = 0; i < _clients.size(); i++)
	{
		toSend.append(_clients[i]->getNickname());
		if (i < _clients.size() - 1)
			toSend.append(" ");
	}
	toSend.append("\r\n");
	send(sendTo->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	toSend = ":ircserv 366 ";
	toSend.append(sendTo->getNickname());
	toSend.push_back(' ');
	toSend.append(_name);
	toSend.append(" :End of NAMES List\r\n");
	send(sendTo->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
}

void	Channel::RemoveClient(Client* toRemove)
{
	if (!ClientIsInChannel(toRemove))
		return ;
	{
		std::vector<Client*>::iterator it;
		for (it = _clients.begin(); *it != toRemove; it++) ;
		_clients.erase(it);
		std::vector<Channel*>::iterator it2;
		for (it2 = toRemove->joined_channels.begin(); *it2 != this; it2++) ;
		toRemove->joined_channels.erase(it2);
	}
}

int Channel::LeaveChannel(Client* toLeave, std::string reason)
{
	std::string toSend;
	if (!ClientIsInChannel(toLeave))
	{
		toSend = ":ircserv 442 ";
		toSend.append(toLeave->getNickname());
		toSend.push_back(' ');
		toSend.append(_name);
		toSend.append(" :You are not on the channel\r\n");
		send(toLeave->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return 1;
	}
	{
		std::vector<Client*>::iterator it;
		for (it = _clients.begin(); *it != toLeave; it++) ;
		_clients.erase(it);
		std::vector<Channel*>::iterator it2;
		for (it2 = toLeave->joined_channels.begin(); *it2 != this; it2++) ;
		toLeave->joined_channels.erase(it2);
	}
	toSend = ":";
	toSend.append(toLeave->getNickname());
	toSend.append(" PART ");
	toSend.append(_name);
	if (!reason.empty())
	{
		toSend.append(" :");
		toSend.append(reason);
	}
	toSend.append("\r\n");
	send(toLeave->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	toSend = ":";
	toSend.append(toLeave->getNickname());
	toSend.append(" PART ");
	toSend.append(_name);
	if (!reason.empty())
	{
		toSend.append(" :");
		toSend.append(reason);
	}
	toSend.append("\r\n");
	for (size_t i = 0; i < _clients.size(); i++)
		send(_clients[i]->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	return 0;
}

void	Channel::SendMessage(Client* sender, std::string message)
{
	std::string toSend = ":";
	if (!ClientIsInChannel(sender))
	{
		toSend.append("ircserv 404 ");
		toSend.append(sender->getNickname());
		toSend.push_back(' ');
		toSend.append(_name);
		toSend.append(" :Cannot send to channel (+n)\r\n");
		send(sender->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return;
	}
	toSend.append(sender->getNickname());
	toSend.append(" PRIVMSG ");
	toSend.append(_name);
	toSend.append(" :");
	toSend.append(message);
	toSend.append("\r\n");
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (*it == sender)
			continue;
		send((*it)->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	}
}

void	Channel::KickAll()
{
	std::string toSend;
	if (this->GetClientCount() < 1)
		return ;
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		Client*	toKick = *it;
		toSend = ":ircserv KICK ";
		toSend.append(_name);
		toSend.push_back(' ');
		toSend.append(toKick->getNickname());
		toSend.append(" :You have been kicked");
		toSend.append("\r\n");
		send(toKick->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	}
	_clients.clear();
}

int	Channel::KickClient(Client* kicker, Client* toKick, std::string reason)
{
	std::string toSend = ":";
	if (!ClientIsInChannel(toKick))
	{
		toSend.append("ircserv 441 ");
		toSend.append(kicker->getNickname());
		toSend.push_back(' ');
		toSend.append(toKick->getNickname());
		toSend.push_back(' ');
		toSend.append(_name);
		toSend.append(" :They aren't on that channel\r\n");
		send(kicker->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return 1;
	}
	toSend.append(kicker->getNickname());
	toSend.append(" KICK ");
	toSend.append(_name);
	toSend.push_back(' ');
	toSend.append(toKick->getNickname());
	if (reason.empty())
	{
		toSend.append(" :You have been kicked");
	}
	else
	{
		toSend.append(" :");
		toSend.append(reason);
	}
	toSend.append("\r\n");
	send(toKick->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	std::vector<Client*>::iterator it;
	for (it = _clients.begin(); *it != toKick; it++) ;
	_clients.erase(it);
	std::vector<Channel*>::iterator it2;
	for (it2 = toKick->joined_channels.begin(); *it2 != this; it2++) ;
	toKick->joined_channels.erase(it2);
	return 0;
}

int	Channel::ChangeTopic(Client* changer, std::string newTopic)
{
	_topic = newTopic;
	std::string toSend = ":";
	toSend.append(changer->getNickname());
	toSend.push_back(' ');
	toSend.append(" TOPIC ");
	toSend.append(_name);
	toSend.append(" :");
	toSend.append(newTopic);
	toSend.append("\r\n");
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		send((*it)->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	return 0;
}

std::string	Channel::GetTopic()
{
	return _topic;
}

std::string	Channel::GetName()
{
	return _name;
}

size_t Channel::GetClientCount()
{
	return _clients.size();
}

bool Channel::ClientIsInChannel(Client* client)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
			return true;
	}
	return false;
}