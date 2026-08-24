#include "../../include/server.hpp"
#include <algorithm>

std::string Server::handleInviteCommand(Client &client, request &p)
{
    if (p.arg.size() < 2)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return "";
    }

    Channel *chan = findChannelByName(p.arg[1]);

    if (chan == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[1]));
        return "";
    }

    if (std::find(client._channel.begin(), client._channel.end(), p.arg[1]) == client._channel.end())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[1]));
        return "";
    }

    if (chan->inviteOnly && !isClientChannelOperator(chan, client))
    {
        sendMessageToOneClient(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[1]));
        return "";
    }

    Client *target = NULL;
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->second.nickName == p.arg[0])
        {
            target = &it->second;
            break;
        }
    }

    if (target == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHNICK(p.arg[0]));
        return "";
    }

    for (std::vector<Client *>::iterator m = chan->_members.begin(); m != chan->_members.end(); ++m)
    {
        if (*m == target)
        {
            sendMessageToOneClient(client.socket_fd, ERR_USERONCHANNEL(p.arg[0], p.arg[1]));
            return "";
        }
    }

    if (std::find(chan->invitedUsers.begin(), chan->invitedUsers.end(), p.arg[0]) == chan->invitedUsers.end())
        chan->invitedUsers.push_back(p.arg[0]);

    sendMessageToOneClient(client.socket_fd, RPL_INVITING(client.nickName, p.arg[1], p.arg[0]));
    sendMessageToOneClient(target->socket_fd, ":" + client.nickName + "!" + client.userName
        + "@localhost INVITE " + p.arg[0] + " :" + p.arg[1] + "\r\n");

    return ("");
}
