#include "../../include/server.hpp"

void Server::sendPrivateMessageToClient(request& req, Client& cli, int client_dest)
{
    std::string msg;
    std::string str;

    if (client_dest == -1)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOSUCHNICK(req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOTEXTTOSEND());
        return;
    }

    str = req.arg[1];

    msg = ":" + cli.nickName + "!~" + cli.userName + "@localhost PRIVMSG " + req.arg[0] + " :" + str + "\r\n";
    queueMessage(client_dest, msg);
}

void Server::sendPrivateMessageToChannel(Client& cli, request& req)
{
    std::string msg;
    std::string str;
    bool etat = false;
    std::vector<Client*>::iterator it;

    std::map<std::string, Channel*>::iterator chanIt = channels.find(req.arg[0]);

    if (chanIt == channels.end() || chanIt->second == NULL)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOSUCHCHANNEL(req.arg[0]));
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
        sendMessageToOneClient(cli.socket_fd, ERR_NOTONCHANNEL(cli.nickName, req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NOTEXTTOSEND());
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
