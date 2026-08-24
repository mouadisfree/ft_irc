#include "../../include/server.hpp"
#include <algorithm>

void Server::handleModeCommand(Client& cli, request& req)
{

    if (req.arg.empty())
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NEEDMOREPARAMS(req.cmd));
        return;
    }

    if (req.arg[0] == cli.nickName)
    {
        if (req.arg.size() >= 2)
            sendMessageToOneClient(cli.socket_fd, ":" + cli.nickName + " MODE " + req.arg[0] + " " + req.arg[1] + "\r\n");
        return;
    }

    Channel *chan = findChannelByName(req.arg[0]);

    if (chan == NULL)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOSUCHCHANNEL(req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        std::string modes = "+";

        if (chan->inviteOnly)   modes += "i";
        if (chan->changeTopic)  modes += "t";
        if (chan->hasPassword)  modes += "k";
        if (chan->isLimit)      modes += "l";

        sendMessageToOneClient(cli.socket_fd, ":irc.server.com 324 " + cli.nickName + " " + req.arg[0] + " " + modes + "\r\n");
        return;
    }

    if (std::find(cli._channel.begin(), cli._channel.end(), req.arg[0]) == cli._channel.end())
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOTONCHANNEL(cli.nickName, req.arg[0]));
        return;
    }

    if (!isClientChannelOperator(chan, cli))
    {
        sendMessageToOneClient(cli.socket_fd, ERR_CHANOPRIVSNEEDED(req.arg[0]));
        return;
    }

    const std::string &mode = req.arg[1];

    bool needsParam = (mode == "+k" || mode == "+o" || mode == "-o" || mode == "+l");

    if (needsParam && req.arg.size() < 3)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NEEDMOREPARAMS(req.cmd));
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
            sendMessageToOneClient(cli.socket_fd, ERR_INVALIDMODEPARAM(req.arg[0]));
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
            sendMessageToOneClient(cli.socket_fd, ERR_USERNOTINCHANNEL(param, req.arg[0]));
            return;
        }

        if (mode == "+o")
        {
            if (!isClientChannelOperator(chan, *target))
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
        sendMessageToOneClient(cli.socket_fd, ERR_INVALIDMODEPARAM(req.arg[0]));
        return;
    }

    sendMessageToChannelMembers(echo + "\r\n", req.arg[0]);
}
