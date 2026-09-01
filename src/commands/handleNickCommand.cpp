#include "../../include/server.hpp"

void Server::handleNickCommand(Client& cli, request &p)
{
    bool etat = true;
    std::map<int,Client>::iterator it;

    if (p.arg.empty())
    {
        sendMessageToOneClient(cli.socket_fd, ERR_NONICKNAMEGIVEN());
        cli.count = 1;
    }
    else
    {
        for (it = clients.begin(); it != clients.end(); ++it)
        {
            if (it->second.nickName == p.arg[0])
            {
                etat = false;
                cli.count = 1;
                sendMessageToOneClient(cli.socket_fd, ERR_NICKNAMEINUSE(p.arg[0]));
            }
        }
        if (etat == true)
        {
            cli.nickName = p.arg[0];
            clients[cli.socket_fd] = cli;
            sendMessageToOneClient(cli.socket_fd, ":irc.server.com NOTICE * :Nickname set, please send USER\r\n");
        }
    }
}
