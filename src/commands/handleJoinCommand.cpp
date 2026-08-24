#include "../../include/server.hpp"
#include <algorithm>
#include <iostream>

std::string Server::handleJoinCommand(Client &client, request &p)
{
    static int user_size = 0;

    if (p.arg.empty() || p.arg[0].empty())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return "";
    }

    user_size += 1;
    std::map<std::string, Channel *>::iterator it = channels.find(p.arg[0]);

    if (p.arg[0][0] != '#')
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return "";
    }
    else if ((std::find(client._channel.begin(), client._channel.end(), p.arg[0])) != client._channel.end())
    {
        sendMessageToOneClient(client.socket_fd, RPL_ALREAYREGISTRED(client.nickName, p.arg[0]));
        return "";
    }
    else if (it == channels.end())
    {
        std::cout << "creating channel " << std::endl;
        createNewChannelAndJoinCreator(p.arg[0], client, p);
        return "";
    }
    else if (isChannelMemberLimitReached(p, &user_size) == true)
    {
        sendMessageToOneClient(client.socket_fd, ERR_CHANNELISFULL(client.nickName, p.arg[0]));
        return "";
    }
    else
    {
        Channel *chan = (*it).second;

        std::vector<std::string>::iterator inv =
            std::find(chan->invitedUsers.begin(), chan->invitedUsers.end(), client.nickName);

        if (chan->inviteOnly && inv == chan->invitedUsers.end())
        {
            sendMessageToOneClient(client.socket_fd, ERR_INVITEONLYCHAN(client.nickName, p.arg[0]));
            return "";
        }

        if (inv != chan->invitedUsers.end())
            chan->invitedUsers.erase(inv);

        if (validateChannelKeyAndJoinClient(client, p, it) == 1)
            return "";
    }
    return "";
}
