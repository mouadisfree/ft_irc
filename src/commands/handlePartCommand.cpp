#include "../../include/server.hpp"
#include <algorithm>

void Server::handlePartCommand(Client &client, request &p)
{
    if (p.arg.empty() || p.arg[0].empty())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return;
    }

    Channel *chan = findChannelByName(p.arg[0]);

    if (chan == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return;
    }

    bool isMember = false;
    for (std::vector<Client*>::iterator m = chan->_members.begin(); m != chan->_members.end(); ++m)
    {
        if (*m == &client)
        {
            isMember = true;
            break;
        }
    }

    if (!isMember)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return;
    }

    std::string reason = (p.arg.size() > 1) ? p.arg[1] : client.nickName;
    std::string msg = ":" + client.nickName + "!" + client.userName
        + "@localhost PART " + p.arg[0] + " :" + reason + "\r\n";

    sendMessageToChannelMembers(msg, p.arg[0]);

    for (std::vector<Client*>::iterator m = chan->_members.begin(); m != chan->_members.end(); )
    {
        if (*m == &client)
            m = chan->_members.erase(m);
        else
            ++m;
    }

    for (std::vector<Client*>::iterator a = chan->admins.begin(); a != chan->admins.end(); )
    {
        if (*a == &client)
            a = chan->admins.erase(a);
        else
            ++a;
    }

    std::vector<std::string>::iterator ch =
        std::find(client._channel.begin(), client._channel.end(), p.arg[0]);
    if (ch != client._channel.end())
        client._channel.erase(ch);

    if (chan->_members.empty())
    {
        std::map<std::string, Channel*>::iterator dead = channels.find(p.arg[0]);

        if (dead != channels.end())
        {
            delete dead->second;
            channels.erase(dead);
        }
        return;
    }

    if (chan->admin == &client)
    {
        chan->admin = chan->_members[0];

        if (!isClientChannelOperator(chan, *chan->admin))
            chan->admins.push_back(chan->admin);
    }
}
