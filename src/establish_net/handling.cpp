#include "../../headers/server.hpp"
#include <string>

void Server::queueMessage(int sockfd, const std::string &message)
{
    std::map<int, Client>::iterator it = clients.find(sockfd);

    if (it == clients.end())
        return;

    it->second.outBuffer += message;
}

void Server::flushClient(Client &client)
{
    if (client.outBuffer.empty())
        return;

    ssize_t sent = send(client.socket_fd, client.outBuffer.c_str(), client.outBuffer.size(), 0);

    if (sent > 0)
    {
        client.outBuffer.erase(0, static_cast<size_t>(sent));
        return;
    }

    if (sent < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return;

    client.step = C_CLOSE_CONNECTION;
}

void Server::send_message(int sockfd, const std::string& message)
{
    queueMessage(sockfd, message);
}

void Server::send_all_member(int sockfd, const std::string &message)
{
    std::map<int, Client>::iterator it = clients.begin();
    for (; it != clients.end();it++)
    {
        if (it->second.socket_fd != sockfd)
            queueMessage(it->second.socket_fd, message);
    }
}

void Server::send_just_member(const std::string &message, std::string channel)
{
    std::map<std::string, Channel*>::iterator chanIt = channels.find(channel);

    if (chanIt == channels.end())
        return;

    std::vector<Client*>::iterator it = chanIt->second->_members.begin();
    for (; it != chanIt->second->_members.end(); it++)
        queueMessage((*it)->socket_fd, message);
}

bool Server::isAdmin(request& req, Client& cli)
{
    std::map<std::string, Channel*>::iterator chanIt = channels.find(req.arg[1]);

    if (chanIt == channels.end())
        return false;

    std::vector<Client*>::iterator it;
    for (it = chanIt->second->admins.begin(); it != chanIt->second->admins.end(); ++it)
    {
        if ((*it)->nickName == cli.nickName)
            return true;
    }
    return false;
}
