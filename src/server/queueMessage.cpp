#include "../../include/server.hpp"

void Server::queueMessage(int sockfd, const std::string &message)
{
    std::map<int, Client>::iterator it = clients.find(sockfd);

    if (it == clients.end())
        return;

    it->second.outBuffer += message;
}
