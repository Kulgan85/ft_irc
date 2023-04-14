#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Channel
{
	public:
	Channel(std::string name);
	~Channel();
	int	JoinChannel(Client* toJoin);
	int	LeaveChannel(Client* toLeave);
	int	KickClient(Client* toKick);
	int	ChangeTopic(std::string newTopic);

	private:
	Channel();

	const std::string 		_name;
	std::vector<Client*>	_clients;
	std::string				_topic;
	std::vector<std::string>	_operatorIPs;
	std::vector<std::string>	_bannedIPs;

};

#endif