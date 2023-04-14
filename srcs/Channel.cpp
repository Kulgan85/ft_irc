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
	if (ClientIsBanned(toJoin))
	{
		toSend = ":ircserv 474 ";
		toSend.append(toJoin->getNickname());
		toSend.push_back(' ');
		toSend.append(_name);
		toSend.append(" :Cannot join channel (+b)\r\n");
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
	return 0;
}

int Channel::JoinChannel(int sender_fd)
{

}

int Channel::LeaveChannel(Client* toLeave)
{

}

int	Channel::LeaveChannel(int sender_fd, std::string reason)
{

}

int	Channel::KickClient(Client* toKick)
{

}

int	Channel::BanClient(Client* toBan)
{
	if (ClientIsInChannel(toBan))
		KickClient(toBan);
	_bannedIPs.push_back(toBan->getIPAddress());
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

bool	Channel::ClientIsBanned(Client* client)
{
	for (int i = 0; i < _bannedIPs.size(); i++)
	{
		if (_bannedIPs[i] == client->getIPAddress())
			return true;
	}
	return false;
}