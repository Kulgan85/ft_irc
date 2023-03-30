#include <iostream>
#include <vector>
#include <string>

struct Channel {
    std::string name;
    std::vector<int> clients;
};

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
