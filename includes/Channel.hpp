#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	Channel(std::string name);
	~Channel();
	int		JoinChannel(Client* toJoin);
	int		LeaveChannel(Client* toLeave, std::string reason);
	void	SendMessage(Client* sender, std::string message);
	int		KickClient(Client* kicker, Client* toKick, std::string reason);
	int		ChangeTopic(std::string newTopic);
	bool	ClientIsInChannel(Client* client);

	private:
	Channel();

	const std::string 		_name;
	std::vector<Client*>	_clients;
	std::string				_topic;
};

#endif