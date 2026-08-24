#include "../../include/server.hpp"

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
