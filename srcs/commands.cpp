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

struct Channel {
    std::string name;
    std::vector<int> clients;
};

void	Server::JOIN(const int &sender_fd)
{
	Channel channel;
	std::vector<std::string> channel_name = this->_splitString(this->_clients[sender_fd]->getMessage);
	if (channel_name.empty() == true)
		return ;
	if (channel_name.front != '#' || channel_name.front != '&')
	{
		std::cout << "Error: Channel name must start with a # or an &" << std::endl;
		return ; 
	}
	for (const auto& channel : channels)
	{
		if (channel.name == channel_name) 
		{
			join_channel(sender_fd, channel_name);
		}
		
	}
}



std::vector<Channel> channels;

// Create a new channel with the given name
void create_channel(std::string name) {
    Channel channel;
    channel.name = name;
    channels.push_back(channel);
}

// Add a client to a channel
void join_channel(int client_fd, std::string channel_name) {
    for (auto& channel : channels) {
        if (channel.name == channel_name) {
            channel.clients.push_back(client_fd);
            std::cout << "Client " << client_fd << " joined channel " << channel_name << "\n";
            return;
        }
    }
    std::cout << "Channel " << channel_name << " does not exist\n";
}

// Remove a client from a channel
void leave_channel(int client_fd, std::string channel_name) {
    for (auto& channel : channels) {
        if (channel.name == channel_name) {
            auto it = std::find(channel.clients.begin(), channel.clients.end(), client_fd);
            if (it != channel.clients.end()) {
                channel.clients.erase(it);
                std::cout << "Client " << client_fd << " left channel " << channel_name << "\n";
                return;
            }
        }
    }
    std::cout << "Client " << client_fd << " is not in channel " << channel_name << "\n";
}

// void	Server::JOIN(const int &sender_fd) {(void)sender_fd;}
// 	std::unordered_map<std::string, /*Nickname??*/ channels;
// 	std::string channel_name = message.substr(5, message.find("\r\n") - 5);
// 	std::string to_send;
// 	auto channel_it = channels.find(channel_name);
// 	if (channel_it == channels.end())
// 	{
// 		channels.insert(std::make_pair(channel_name, /*Nickname??*/));
// 		to_send = "Channel ";
// 		to_send.append(channel_name);
// 		to_send.append(" created!\r\n")
// 		send(sender_fd, to_send.c_str(), to_send.size(), MSG_DONTWAIT);
// 	}
// 	else
// 	{
// 		channel_it->second.push_back(new_user);
// 	}
// 	std::out << this->_clients[sender_fd]->getNickname() << " has joined " << channel_name << "\r\n";

// void USER::JOIN(std::string target)
// {
// 	std::vector<std::string>			target_split = split(target, ',');
// 	std::vector<std::string>::iterator	it = target_split.begin();
// 	std::vector<Channel>::iterator		channel;
// 	std::string 						msg;

// 	if (target[0] == '0' && target.length() == 1)
// 		return part("", ":*");
// 	while (it != target_split.end())
// 	{
// 		channel = this->get_channel(*it);
// 		msg = ERR_BADCHANMASK + *it + " :Channel names have to start with #\r\n";
// 		if (it[0][0] != '#')
// 			send(this->_fd, msg.c_str(), msg.length(), 0);
// 		else if (channel == this->_serv->channels.end())
// 			this->_serv->channels.push_back(Channel(*it, &get_user(this->_nick_name)->second));
// 		else
// 			channel->join(&get_user(this->_nick_name)->second);
// 		++it;
// 	}
// }

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

void	Server::LEAVE(const int &sender_fd) {(void)sender_fd;}

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
