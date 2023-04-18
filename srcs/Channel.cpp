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
		//TODO: What the fuck do we do in this situation?
		std::cerr << "Client already joined";
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
	toSend = ":ircserv 353 ";
	toSend.append(toJoin->getNickname());
	toSend.append(" = ");
	toSend.append(_name);
	toSend.append(" :");
	for (int i = 0; i < _clients.size(); i++)
	{
		toSend.append(_clients[i]->getNickname());
		if (i < _clients.size() - 1)
			toSend.append(" ");
	}
	toSend.append("\r\n");
	send(toJoin->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	toSend = ":ircserv 366 ";
	toSend.append(toJoin->getNickname());
	toSend.push_back(' ');
	toSend.append(_name);
	toSend.append(" :End of NAMES List\r\n");
	send(toJoin->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	toSend = ":";
	toSend.append(toJoin->getNickname());
	toSend.append(" JOIN ");
	toSend.append(_name);
	toSend.append("\r\n");
	for (size_t i = 0; i < _clients.size() - 1; i++)
		send(_clients[i]->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
	return 0;
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
	toSend = ":ircserv PART ";
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

int	Channel::KickClient(Client* kicker, Client* toKick, std::string reason)
{
	std::string toSend;
	if (!ClientIsInChannel(toKick))
	{
		toSend = ":ircserv 441 ";
		toSend.append(kicker->getNickname());
		toSend.push_back(' ');
		toSend.append(toKick->getNickname());
		toSend.push_back(' ');
		toSend.append(_name);
		toSend.append(" :They aren't on that channel\r\n");
		send(kicker->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return 1;
	}

}

int	Channel::ChangeTopic(std::string newTopic)
{
	_topic = newTopic;
}

bool Channel::ClientIsInChannel(Client* client)
{
	for (int i = 0; i < _clients.size(); i++)
	{
		if (_clients[i] == client)
			return true;
	}
	return false;
}