#include "../../include/server.hpp"

void Server::sendMessageToChannelMembers(const std::string &message, std::string channel)
{
    std::map<std::string, Channel*>::iterator chanIt = channels.find(channel);

    if (chanIt == channels.end())
        return;

    std::vector<Client*>::iterator it = chanIt->second->_members.begin();
    for (; it != chanIt->second->_members.end(); it++)
        queueMessage((*it)->socket_fd, message);
}
