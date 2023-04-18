#include "Server.hpp"

std::vector<std::string> Server::_splitString(std::string str)
{
	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		std::cout << (int)str[i] << " ";
	}
	std::cout << std::endl;
	if (str.at(str.length() - 1) == '\n')
		str.erase(str.length() - 1);
	if (str.at(str.length() - 1) == '\r')
		str.erase(str.length() - 1);
	std::vector<std::string> tokens;
	std::string	buf;

	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		buf.clear();
		if (str[i] == ':')
		{
// This next line removes the ':' character from the start of the last parameter
			++i;
			while (i < str.size())
			{
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
		else if (str[i] != ' ')
		{
			while (str[i] != ' ' && i < str.size())
			{
				if (!(Server::_isValidChar(str[i])))
				{
					if (i == 0 && str[i] == '*' && tokens.size() == 2 && tokens[0].compare("USER") == 0)
						;
					else
					{
						std::cout << "Invalid character is |" << (int)str[i] << "|\n"; 
						tokens.clear();
						return (tokens);
					}
				}
				buf.push_back(str[i]);
				++i;
			}
			tokens.push_back(buf);
		}
	}
	return (tokens);
}

void	Server::JOIN(const int &sender_fd)
{
	std::vector<std::string> user_input = this->_splitString(this->_clients[sender_fd]->getMessage());
	
	if (user_input[2].empty() == true)
		return ;
	if (user_input[2][0] != '#' || user_input[2][0] != '&')
	{
		std::cout << "Error: Channel name must start with a # or an &" << std::endl;
		return ; 
	}
	std::map<std::string,Channel*>::iterator it = _channels.find(user_input[2]);
	if (it == _channels.end())
		_createChannel(user_input[2]);
	_joinChannel(user_input[2], sender_fd);
}

// Handles stuff to give to the actualy join channel
void Server::_joinChannel(std::string channel_name, int sender_fd) {
    Channel *channelptr = _channels.find(channel_name)->second;
	channelptr->JoinChannel(_clients[sender_fd]);
}

void	Server::PASS(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == true)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already input the password)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args.size() < 2)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (this->_password == SHA1Hash(args[1]))
	{
		this->_clients[sender_fd]->setIsVerified(true);
		to_send = ":ircserv 300 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :That was the correct password, you are now verified\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	else
	{
		to_send = ":ircserv 464 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Incorrect password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
}

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

//[2] = channel names
//[3] = reason
void	Server::LEAVE(const int &sender_fd) 
{	
	Client*						client = _clients[sender_fd];
	std::vector<std::string>	user_input = this->_splitString(client->getMessage());
	if (user_input[2].empty())
	{
		std::string toSend = ":ircserv 461 ";
		toSend.append(client->getNickname());
		toSend.append(" PART :Not enough parameters\r\n");
		send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string> target_channels = getTargets(user_input[2]);
	for (int i = 0; i < target_channels.size(); i++)
	{
		std::map<std::string,Channel*>::iterator it = _channels.find(target_channels[i]);
		if (it != _channels.end())
			it->second->LeaveChannel(client, user_input[3]);
		else
		{
			std::string toSend = ":ircserv 403 ";
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(target_channels[i]);
			toSend.append(" :No such channel\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
		}
	}
}

void	Server::TOPIC(const int& sender_fd)
{
		std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
		std::string toSend = ":";
		Client*	client = _clients[sender_fd];
		if (args.size() < 3)
		{
			toSend.append("ircserv 461 ");
			toSend.append(client->getNickname());
			toSend.append(" TOPIC :Not enough parameters\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			return;
		}
		Channel*	channel;
		try
		{
			channel = _channels.at(args[2]);
		}
		catch(const std::exception& e)
		{
			toSend.append("ircserv 403 ");
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(args[2]);
			toSend.append(" :No such channel\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			return;
		}
		if (!channel->ClientIsInChannel(client))
		{
			toSend.append("ircserv 442 ");
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(args[2]);
			toSend.append(" :You're not on that channel\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			return;
		}
		if (args.size() == 3)
		{
			std::string topic = channel->GetTopic();
			if (topic.empty())
				{ toSend.append("ircserv 331 "); topic = "No topic is set"; }
			else
				toSend.append("ircserv 332 ");
			toSend.append(client->getNickname());
			toSend.push_back(' ');
			toSend.append(args[2]);
			toSend.append(" :");
			toSend.append(topic);
			toSend.append("\r\n");
			send(client->getFd(), toSend.c_str(), toSend.size(), MSG_DONTWAIT);
			return;
		}
		//	IDK if I even want channel operators
		//	if (!channel->IsChannelOperator(client))
		//		{ return "fuk off"; }
		channel->ChangeTopic(client, args[3]);
}

void	Server::NAMES(const int& sender_fd)
{

}

void Server::_joinChannel(std::string channel_name, int sender_fd) {
    Channel *channelptr = _channels.find(channel_name)->second;
	channelptr->JoinChannel(_clients[sender_fd]);
}
void	Server::OPER(const int &sender_fd)
{
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsRegistered() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not registered\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (this->_clients[sender_fd]->getIsOperator() == true)
	{
		to_send = ":ircserv 300 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You are already an operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 3)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (args[1] != this->_clients[sender_fd]->getNickname())
		return ;
	if (args[2] == this->_oper_password)
	{
		this->_clients[sender_fd]->setIsOperator(true);
		to_send = ":ircserv 381 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You are now an IRC operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		to_send.clear();
		to_send = ":ircserv 221 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" +");
		if (this->_clients[sender_fd]->getIsOperator() == true)
			to_send.push_back('o');
		to_send.append("\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	else
	{
		to_send = ":ircserv 464 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Password incorrect\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
}

void	Server::NICK(const int &sender_fd)
{
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not input the password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 2)
	{
		to_send = ":ircserv 431 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :No nickname given\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (!Server::_isValidNick(args[1]))
	{
		to_send = ":ircserv 432 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Erroneous nickname\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (std::find(this->_nicknames.begin(), this->_nicknames.end(), args[1]) != this->_nicknames.end())
	{
		to_send = ":ircserv 433 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[1]);
		to_send.append(" :Nickname is already in use\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	to_send = ":";
	if (find(this->_nicknames.begin(), this->_nicknames.end(), this->_clients[sender_fd]->getNickname()) != this->_nicknames.end())
		this->_nicknames.erase(find(this->_nicknames.begin(), this->_nicknames.end(), this->_clients[sender_fd]->getNickname()));
	to_send.append(this->_clients[sender_fd]->getNickname());
	this->_clients[sender_fd]->setNickname(args[1]);
	this->_nicknames.push_back(args[1]);
	to_send.append(" NICK ");
	to_send.append(args[1]);
	to_send.append("\r\n");
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		send(it->first, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	}
	to_send = ":";
	to_send.append(this->_name);
	to_send.append(" NICK change successful: ");
	to_send.append(args[1]);
	to_send.append("\r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
	if (this->_clients[sender_fd]->getUsername().size() > 0 && this->_clients[sender_fd]->getIsRegistered() == false)
	{
		this->_clients[sender_fd]->setIsRegistered(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::USER(const int &sender_fd)
{
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsVerified() == false)
	{
		to_send = ":ircserv 451 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :You have not input the password\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	if (this->_clients[sender_fd]->getUsername().size() > 0)
	{
		to_send = ":ircserv 462 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Unauthorized command (already registered)\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.empty() == true)
		return ;
	if (args.size() < 4)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" ");
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	this->_clients[sender_fd]->setUsername(args[1]);
	if (this->_clients[sender_fd]->getNickname().compare("*") != 0 && this->_clients[sender_fd]->getIsRegistered() == false)
	{
		this->_clients[sender_fd]->setIsRegistered(true);
		Server::_sendWelcome(sender_fd);
	}
}

void	Server::QUIT(const int &sender_fd)
{
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	std::string	quit_message = ":";
	quit_message.append(this->_clients[sender_fd]->getNickname());
	quit_message.push_back('!');
	quit_message.append(this->_clients[sender_fd]->getUsername());
	quit_message.push_back('@');
	quit_message.append(this->_name);
	quit_message.push_back(' ');
	quit_message.append(args[0]);
	if (args.size() > 1)
	{
		quit_message.append(" :");
		quit_message.append(args[1]);
	}
	quit_message.append("\r\n");
	std::string	error_message = "ERROR :";
	error_message.append(this->_name);
	error_message.append(" you issued a QUIT command\r\n");
	send(sender_fd, error_message.c_str(), error_message.size(), MSG_DONTWAIT);

	int	pfds_index = this->_pfd_count - 1;

	while (this->_pfds[pfds_index].fd != sender_fd && pfds_index >= 0)
		--pfds_index;
	if (pfds_index < 0)
	{
		std::cout << "ERROR: Something is very wrong" << std::endl;
	}
	close(sender_fd);
	Server::_removeFromPoll(pfds_index);
	Server::_removeClient(sender_fd);
	std::cout << "Socket " << sender_fd << " hung up" << std::endl;
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		send(it->second->getFd(), quit_message.c_str(), quit_message.size(), MSG_DONTWAIT);
	}
}

void	Server::KILL(const int &sender_fd)
{
	std::string	to_send;
	if (this->_clients[sender_fd]->getIsOperator() == false)
	{
		to_send = ":ircserv 481 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.append(" :Permission Denied- You're not an IRC operator\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	std::vector<std::string>	args = Server::_splitString(this->_clients[sender_fd]->getMessage());
	if (args.size() < 3)
	{
		to_send = ":ircserv 461 ";
		to_send.append(this->_clients[sender_fd]->getNickname());
		to_send.push_back(' ');
		to_send.append(args[0]);
		to_send.append(" :Not enough parameters\r\n");
		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
		return ;
	}
	for (std::map<int, Client *>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->second->getNickname() == args[1])
		{
			std::string	kill_message = ":";
			kill_message.append(this->_name);
			kill_message.append(" KILL ");
			kill_message.append(args[1]);
			kill_message.append(" :");
			kill_message.append(args[2]);
			for (std::map<int, Client *>::iterator it2 = this->_clients.begin(); it2 != this->_clients.end(); it2++)
				send(it2->first, kill_message.c_str(), kill_message.size(), MSG_DONTWAIT);

			int	pfds_index = this->_pfd_count - 1;
			while (this->_pfds[pfds_index].fd != it->first && pfds_index >= 0)
				--pfds_index;
			close(it->first);
			Server::_removeFromPoll(pfds_index);
			Server::_removeClient(it->first);
			return ;
		}
	}
	to_send = ":ircserv 401 ";
	to_send.append(this->_clients[sender_fd]->getNickname());
	to_send.push_back(' ');
	to_send.append(args[1]);
	to_send.append(" :No such nick\r\n");
	send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
}

void	Server::LIST(const int &sender_fd) {(void)sender_fd;}
