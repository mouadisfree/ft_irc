#include "../../include/server.hpp"

void Server::sendMessageToAllClientsExceptSender(int sockfd, const std::string &message)
{
    std::map<int, Client>::iterator it = clients.begin();
    for (; it != clients.end();it++)
    {
        if (it->second.socket_fd != sockfd)
            queueMessage(it->second.socket_fd, message);
    }
}
