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
	void	RemoveClient(Client* toRemove);
	int		ChangeTopic(Client* changer, std::string newTopic);
	bool	ClientIsInChannel(Client* client);
	void	SendNames(Client* sendTo);
	void	KickAll();

	std::string	GetTopic();
	std::string	GetName();
	size_t		GetClientCount();

	private:
	Channel();

	const std::string 		_name;
	std::vector<Client*>	_clients;
	std::string				_topic;
};

#endif