#include "../../include/server.hpp"

void Server::handleUserCommand(Client& cli, request &p)
{
    if (p.arg.size() < 4)
    {
        cli.count = 2;
        sendMessageToOneClient(cli.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return;
    }

    cli.userName = p.arg[0];
    cli.hostName = p.arg[1];
    cli.serverName = p.arg[2];
    cli.realName = p.arg[3];
}
