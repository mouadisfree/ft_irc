#include "../../include/server.hpp"
#include <algorithm>

std::string Server::handleKickCommand(Client &client, request &p)
{
    if (p.arg.size() < 2)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return ("");
    }

    Channel *chan = findChannelByName(p.arg[0]);

    if (chan == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return ("");
    }

    if (std::find(client._channel.begin(), client._channel.end(), p.arg[0]) == client._channel.end())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return ("");
    }

    if (!isClientChannelOperator(chan, client))
    {
        sendMessageToOneClient(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[0]));
        return ("");
    }

    if (client.nickName == p.arg[1])
    {
        sendMessageToOneClient(client.socket_fd, ERR_KICKYOUSELF(client.nickName));
        return ("");
    }

    for (std::vector<Client *>::iterator it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if (p.arg[1] != (*it)->nickName)
            continue ;

        Client *target = *it;

        sendMessageToChannelMembers(KICKUSER(client.nickName, client.userName, p.arg[0], p.arg[1]), p.arg[0]);

        std::vector<std::string>::iterator ch =
            std::find(target->_channel.begin(), target->_channel.end(), p.arg[0]);
        if (ch != target->_channel.end())
            target->_channel.erase(ch);

        chan->_members.erase(it);

        for (std::vector<Client*>::iterator a = chan->admins.begin(); a != chan->admins.end(); )
        {
            if (*a == target)
                a = chan->admins.erase(a);
            else
                ++a;
        }

        if (chan->admin == target)
            chan->admin = chan->_members.empty() ? NULL : chan->_members[0];

        return ("");
    }

    sendMessageToOneClient(client.socket_fd, ERR_NOSUCHNICK(p.arg[1]));
    return ("");
}
