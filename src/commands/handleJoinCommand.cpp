#include "../../include/server.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

void Server::sendMessageToChannelMembers(const std::string &message, std::string channel)
{
    std::map<std::string, Channel*>::iterator chanIt = channels.find(channel);

    if (chanIt == channels.end())
        return;

    std::vector<Client*>::iterator it = chanIt->second->_members.begin();
    for (; it != chanIt->second->_members.end(); it++)
        queueMessage((*it)->socket_fd, message);
}

void Server::sendChannelTopicReply(Client &t, Channel *chan)
{
    if (chan->getTopic().empty())
    {
        sendMessageToOneClient(t.socket_fd, ":irc.server.com 331 " + t.nickName + " "
            + chan->_name + " :No topic is set\r\n");
        return;
    }

    std::stringstream when;
    when << chan->topicTime;

    sendMessageToOneClient(t.socket_fd, RPL_TOPIC(t.nickName, chan->_name, chan->getTopic()));
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 333 " + t.nickName + " " + chan->_name
        + " " + chan->topicSetter + " " + when.str() + "\r\n");
}

bool	Server::isClientChannelOperator(Channel *chan, Client &cli)
{
    if (chan == NULL)
        return false;

    for (std::vector<Client*>::iterator it = chan->admins.begin(); it != chan->admins.end(); ++it)
    {
        if (*it == &cli)
            return true;
    }
    return false;
}

bool Server::isChannelMemberLimitReached(request& req, int *user)
{
    (void)user;

    Channel *chan = findChannelByName(req.arg[0]);

    if (chan == NULL || !chan->isLimit)
        return false;

    return (static_cast<int>(chan->_members.size()) >= chan->maxsize);
}

Channel	*Server::findChannelByName(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);

    if (it == channels.end())
        return NULL;
    return it->second;
}

void Server::createNewChannelAndJoinCreator(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = new Channel(channel, &t);

    channels[channel] = chan;
    chan->changeTopic = true;
    chan->admin = &t;
    chan->admins.push_back(&t);
    t._channel.push_back(channel);

    sendMessageToOneClient(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendChannelTopicReply(t, chan);
    sendMessageToOneClient(t.socket_fd , ":irc.server.com 353 " + t.nickName + " = " + channel + " :@" + t.nickName + "\r\n");
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}


void Server::addClientToExistingChannel(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = findChannelByName(channel);

    if (chan == NULL)
        return;

    chan->_members.push_back(&t);
    t._channel.push_back(channel);

    chan->member_str = ":irc.server.com 353 " + t.nickName + " = " + channel + " :";
    for (std::vector<Client*>::iterator it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if (isClientChannelOperator(chan, **it))
            chan->member_str += "@";
        chan->member_str += (*it)->nickName + " ";
    }
    chan->member_str += "\r\n";

    sendMessageToOneClient(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendChannelTopicReply(t, chan);
    sendMessageToOneClient(t.socket_fd , chan->member_str);
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}

int Server::validateChannelKeyAndJoinClient(Client& client, request& p, std::map<std::string,Channel*>::iterator it)
{
    (void)it;

    Channel *chan = findChannelByName(p.arg[0]);

    if (chan == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return 1;
    }

    if (chan->hasPassword)
    {
        if (p.arg.size() < 2)
        {
            sendMessageToOneClient(client.socket_fd, ERR_BADCHANNELKEY(p.arg[0]));
            return 1;
        }
        if (p.arg[1] != chan->_password)
        {
            sendMessageToOneClient(client.socket_fd, ERR_INVALIDKEY(p.arg[0]));
            return 1;
        }
    }

    addClientToExistingChannel(p.arg[0], client, p);
    sendMessageToChannelMembers(RPL_JOINMSG(client.nickName, client.userName, p.arg[0]), p.arg[0]);
    return 0;
}

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
