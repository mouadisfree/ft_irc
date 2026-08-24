#include "../../include/server.hpp"

int Server::handlePassCommand(Client& client, request &p)
{
    if (p.arg.empty())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        client.count = 0;
    }
    else if (p.arg.size() != 1)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        client.count = 0;
    }
    else if (p.arg[0] != this->password)
    {
        sendMessageToOneClient(client.socket_fd, ERR_PASSWDMISMATCH());
        client.count = 0;
    }
    else
    {
        sendMessageToOneClient(client.socket_fd, ":irc.server.com NOTICE * :Password accepted, please send NICK\r\n");
    }
    return 0;
}
