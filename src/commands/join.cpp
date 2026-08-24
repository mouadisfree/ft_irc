#include "../../headers/server.hpp"
#include <cstddef>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

Channel	*Server::getChannel(const std::string &name)
{
    std::map<std::string, Channel*>::iterator it = channels.find(name);

    if (it == channels.end())
        return NULL;
    return it->second;
}

bool	Server::isOperator(Channel *chan, Client &cli)
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

void Server::sendTopicReply(Client &t, Channel *chan)
{
    if (chan->get_topic().empty())
    {
        send_message(t.socket_fd, ":irc.server.com 331 " + t.nickName + " "
            + chan->_name + " :No topic is set\r\n");
        return;
    }

    std::stringstream when;
    when << chan->topicTime;

    send_message(t.socket_fd, RPL_TOPIC(t.nickName, chan->_name, chan->get_topic()));
    send_message(t.socket_fd, ":irc.server.com 333 " + t.nickName + " " + chan->_name
        + " " + chan->topicSetter + " " + when.str() + "\r\n");
}

void Server::createChannel(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = new Channel(channel, &t);

    channels[channel] = chan;
    chan->changeTopic = true;
    chan->admin = &t;
    chan->admins.push_back(&t);
    t._channel.push_back(channel);

    send_message(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendTopicReply(t, chan);
    send_message(t.socket_fd , ":irc.server.com 353 " + t.nickName + " = " + channel + " :@" + t.nickName + "\r\n");
    send_message(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}

void Server::joinChannel(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = getChannel(channel);

    if (chan == NULL)
        return;

    chan->_members.push_back(&t);
    t._channel.push_back(channel);

    chan->member_str = ":irc.server.com 353 " + t.nickName + " = " + channel + " :";
    for (std::vector<Client*>::iterator it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if (isOperator(chan, **it))
            chan->member_str += "@";
        chan->member_str += (*it)->nickName + " ";
    }
    chan->member_str += "\r\n";

    send_message(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendTopicReply(t, chan);
    send_message(t.socket_fd , chan->member_str);
    send_message(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}

bool Server::checkLimits(request& req, int *user)
{
    (void)user;

    Channel *chan = getChannel(req.arg[0]);

    if (chan == NULL || !chan->isLimit)
        return false;

    return (static_cast<int>(chan->_members.size()) >= chan->maxsize);
}

std::string Server::join(Client &client, request &p)
{
    static int user_size = 0;

    if (p.arg.empty() || p.arg[0].empty())
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return "";
    }

    user_size += 1;
    std::map<std::string, Channel *>::iterator it = channels.find(p.arg[0]);

    if (p.arg[0][0] != '#')
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return "";
    }
    else if ((std::find(client._channel.begin(), client._channel.end(), p.arg[0])) != client._channel.end())
    {
        send_message(client.socket_fd, RPL_ALREAYREGISTRED(client.nickName, p.arg[0]));
        return "";
    }
    else if (it == channels.end())
    {
        std::cout << "creating channel " << std::endl;
        createChannel(p.arg[0], client, p);
        return "";
    }
    else if (checkLimits(p, &user_size) == true)
    {
        send_message(client.socket_fd, ERR_CHANNELISFULL(client.nickName, p.arg[0]));
        return "";
    }
    else
    {
        Channel *chan = (*it).second;

        std::vector<std::string>::iterator inv =
            std::find(chan->invitedUsers.begin(), chan->invitedUsers.end(), client.nickName);

        if (chan->inviteOnly && inv == chan->invitedUsers.end())
        {
            send_message(client.socket_fd, ERR_INVITEONLYCHAN(client.nickName, p.arg[0]));
            return "";
        }

        if (inv != chan->invitedUsers.end())
            chan->invitedUsers.erase(inv);

        if (joinClient(client, p, it) == 1)
            return "";
    }
    return "";
}

int Server::joinClient(Client& client, request& p, std::map<std::string,Channel*>::iterator it)
{
    (void)it;

    Channel *chan = getChannel(p.arg[0]);

    if (chan == NULL)
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return 1;
    }

    if (chan->hasPassword)
    {
        if (p.arg.size() < 2)
        {
            send_message(client.socket_fd, ERR_BADCHANNELKEY(p.arg[0]));
            return 1;
        }
        if (p.arg[1] != chan->_password)
        {
            send_message(client.socket_fd, ERR_INVALIDKEY(p.arg[0]));
            return 1;
        }
    }

    joinChannel(p.arg[0], client, p);
    send_just_member(RPL_JOINMSG(client.nickName, client.userName, p.arg[0]), p.arg[0]);
    return 0;
}

void Server::sendMSGToChannel(Client& cli, request& req)
{
    std::string msg;
    std::string str;
    bool etat = false;
    std::vector<Client*>::iterator it;

    std::map<std::string, Channel*>::iterator chanIt = channels.find(req.arg[0]);

    if (chanIt == channels.end() || chanIt->second == NULL)
    {
        send_message(cli.socket_fd, ERR_NOSUCHCHANNEL(req.arg[0]));
        return;
    }

    Channel *chan = chanIt->second;

    for(it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if ((*it)->socket_fd == cli.socket_fd)
        {
            etat = 1;
            break;
        }
    }

    if (!etat)
    {
        send_message(cli.socket_fd, ERR_NOTONCHANNEL(cli.nickName, req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        send_message(cli.socket_fd, ERR_NOTEXTTOSEND());
        return;
    }

    str = req.arg[1];

    msg = ":" + cli.nickName + "!" + cli.userName + "@localhost PRIVMSG " + req.arg[0] + " :" + str + "\r\n";
    for(it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if ((*it)->socket_fd == cli.socket_fd)
            continue;
        else
            queueMessage((*it)->socket_fd, msg);
    }
}

void Server::part(Client &client, request &p)
{
    if (p.arg.empty() || p.arg[0].empty())
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return;
    }

    Channel *chan = getChannel(p.arg[0]);

    if (chan == NULL)
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
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
        send_message(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return;
    }

    std::string reason = (p.arg.size() > 1) ? p.arg[1] : client.nickName;
    std::string msg = ":" + client.nickName + "!" + client.userName
        + "@localhost PART " + p.arg[0] + " :" + reason + "\r\n";

    send_just_member(msg, p.arg[0]);

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

        if (!isOperator(chan, *chan->admin))
            chan->admins.push_back(chan->admin);
    }
}

void Server::quit(Client &client, request &p)
{
    if (!p.arg.empty() && !p.arg[0].empty())
        client.quitReason = p.arg[0];

    client.step = C_CLOSE_CONNECTION;
}

std::string Server::kick(Client &client, request &p)
{
    if (p.arg.size() < 2)
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return ("");
    }

    Channel *chan = getChannel(p.arg[0]);

    if (chan == NULL)
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return ("");
    }

    if (std::find(client._channel.begin(), client._channel.end(), p.arg[0]) == client._channel.end())
    {
        send_message(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return ("");
    }

    if (!isOperator(chan, client))
    {
        send_message(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[0]));
        return ("");
    }

    if (client.nickName == p.arg[1])
    {
        send_message(client.socket_fd, ERR_KICKYOUSELF(client.nickName));
        return ("");
    }

    for (std::vector<Client *>::iterator it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if (p.arg[1] != (*it)->nickName)
            continue ;

        Client *target = *it;

        send_just_member(KICKUSER(client.nickName, client.userName, p.arg[0], p.arg[1]), p.arg[0]);

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

    send_message(client.socket_fd, ERR_NOSUCHNICK(p.arg[1]));
    return ("");
}

std::string Server::invite(Client &client, request &p)
{
    if (p.arg.size() < 2)
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return "";
    }

    Channel *chan = getChannel(p.arg[1]);

    if (chan == NULL)
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[1]));
        return "";
    }

    if (std::find(client._channel.begin(), client._channel.end(), p.arg[1]) == client._channel.end())
    {
        send_message(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[1]));
        return "";
    }

    if (chan->inviteOnly && !isOperator(chan, client))
    {
        send_message(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[1]));
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
        send_message(client.socket_fd, ERR_NOSUCHNICK(p.arg[0]));
        return "";
    }

    for (std::vector<Client *>::iterator m = chan->_members.begin(); m != chan->_members.end(); ++m)
    {
        if (*m == target)
        {
            send_message(client.socket_fd, ERR_USERONCHANNEL(p.arg[0], p.arg[1]));
            return "";
        }
    }

    if (std::find(chan->invitedUsers.begin(), chan->invitedUsers.end(), p.arg[0]) == chan->invitedUsers.end())
        chan->invitedUsers.push_back(p.arg[0]);

    send_message(client.socket_fd, RPL_INVITING(client.nickName, p.arg[1], p.arg[0]));
    send_message(target->socket_fd, ":" + client.nickName + "!" + client.userName
        + "@localhost INVITE " + p.arg[0] + " :" + p.arg[1] + "\r\n");

    return ("");
}

std::string Server::Topic(Client &client, request &p)
{
    if (p.arg.empty())
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return ("");
    }

    Channel *chan = getChannel(p.arg[0]);

    if (chan == NULL)
    {
        send_message(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return ("");
    }

    bool isMember = false;
    for (std::vector<Client *>::iterator itt = chan->_members.begin(); itt != chan->_members.end(); ++itt)
    {
        if (*itt == &client)
        {
            isMember = true;
            break;
        }
    }

    if (!isMember)
    {
        send_message(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return ("");
    }

    if (p.arg.size() < 2)
    {
        sendTopicReply(client, chan);
        return ("");
    }

    if (chan->changeTopic && !isOperator(chan, client))
    {
        send_message(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[0]));
        return ("");
    }

    chan->set_topic(p.arg[1], client.nickName);

    send_just_member(":" + client.nickName + "!" + client.userName
        + "@localhost TOPIC " + chan->_name + " :" + chan->get_topic() + "\r\n", p.arg[0]);
    return ("");
}

bool Server::is_admin(request& req, Client& cli)
{
    if (req.arg.empty())
        return false;
    return isOperator(getChannel(req.arg[0]), cli);
}

void Server::Mode(Client& cli, request& req)
{

    if (req.arg.empty())
    {
        send_message(cli.socket_fd, ERR_NEEDMOREPARAMS(req.cmd));
        return;
    }

    if (req.arg[0] == cli.nickName)
    {
        if (req.arg.size() >= 2)
            send_message(cli.socket_fd, ":" + cli.nickName + " MODE " + req.arg[0] + " " + req.arg[1] + "\r\n");
        return;
    }

    Channel *chan = getChannel(req.arg[0]);

    if (chan == NULL)
    {
        send_message(cli.socket_fd, ERR_NOSUCHCHANNEL(req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        std::string modes = "+";

        if (chan->inviteOnly)   modes += "i";
        if (chan->changeTopic)  modes += "t";
        if (chan->hasPassword)  modes += "k";
        if (chan->isLimit)      modes += "l";

        send_message(cli.socket_fd, ":irc.server.com 324 " + cli.nickName + " " + req.arg[0] + " " + modes + "\r\n");
        return;
    }

    if (std::find(cli._channel.begin(), cli._channel.end(), req.arg[0]) == cli._channel.end())
    {
        send_message(cli.socket_fd, ERR_NOTONCHANNEL(cli.nickName, req.arg[0]));
        return;
    }

    if (!isOperator(chan, cli))
    {
        send_message(cli.socket_fd, ERR_CHANOPRIVSNEEDED(req.arg[0]));
        return;
    }

    const std::string &mode = req.arg[1];

    bool needsParam = (mode == "+k" || mode == "+o" || mode == "-o" || mode == "+l");

    if (needsParam && req.arg.size() < 3)
    {
        send_message(cli.socket_fd, ERR_NEEDMOREPARAMS(req.cmd));
        return;
    }

    std::string param = (req.arg.size() >= 3) ? req.arg[2] : std::string("");
    std::string echo  = ":" + cli.nickName + " MODE " + req.arg[0] + " " + mode;

    if (mode == "+i" || mode == "-i")
    {
        chan->inviteOnly = (mode == "+i");
    }
    else if (mode == "+t" || mode == "-t")
    {
        chan->changeTopic = (mode == "+t");
    }
    else if (mode == "+k")
    {
        chan->hasPassword = true;
        chan->_password = param;
        echo += " " + param;
    }
    else if (mode == "-k")
    {
        chan->hasPassword = false;
        chan->_password = "";
    }
    else if (mode == "+l")
    {
        int limit = std::atoi(param.c_str());

        if (limit <= 0)
        {
            send_message(cli.socket_fd, ERR_INVALIDMODEPARAM(req.arg[0]));
            return;
        }
        chan->maxsize = limit;
        chan->isLimit = true;
        echo += " " + param;
    }
    else if (mode == "-l")
    {
        chan->isLimit = false;
        chan->maxsize = 0;
    }
    else if (mode == "+o" || mode == "-o")
    {
        Client *target = NULL;

        for (std::vector<Client*>::iterator m = chan->_members.begin(); m != chan->_members.end(); ++m)
        {
            if ((*m)->nickName == param)
            {
                target = *m;
                break;
            }
        }

        if (target == NULL)
        {
            send_message(cli.socket_fd, ERR_USERNOTINCHANNEL(param, req.arg[0]));
            return;
        }

        if (mode == "+o")
        {
            if (!isOperator(chan, *target))
                chan->admins.push_back(target);
        }
        else
        {
            for (std::vector<Client*>::iterator a = chan->admins.begin(); a != chan->admins.end(); )
            {
                if (*a == target)
                    a = chan->admins.erase(a);
                else
                    ++a;
            }
        }
        echo += " " + param;
    }
    else
    {
        send_message(cli.socket_fd, ERR_INVALIDMODEPARAM(req.arg[0]));
        return;
    }

    send_just_member(echo + "\r\n", req.arg[0]);
}
