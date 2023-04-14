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

int	Channel::ChangeTopic(std::string newTopic)
{
	_topic = newTopic;
}