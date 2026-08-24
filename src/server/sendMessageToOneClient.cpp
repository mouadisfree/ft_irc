#include "../../include/server.hpp"

void Server::sendMessageToOneClient(int sockfd, const std::string& message)
{
    queueMessage(sockfd, message);
}
