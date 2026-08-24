#include "../../headers/server.hpp"

int Server::pass(Client& client, request &p)
{
    if (p.arg.empty())
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        client.count = 0;
    }
    else if (p.arg.size() != 1)
    {
        send_message(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        client.count = 0;
    }
    else if (p.arg[0] != this->password)
    {
        send_message(client.socket_fd, ERR_PASSWDMISMATCH());
        client.count = 0;
    }
    else
    {
        send_message(client.socket_fd, ":irc.server.com NOTICE * :Password accepted, please send NICK\r\n");
    }
    return 0;
}

void Server::Nick(Client& cli, request &p)
{
    std::string nick = p.cmd;
    bool etat = true;
    std::map<int,Client>::iterator it;

    if (p.arg.empty())
    {
        send_message(cli.socket_fd, ERR_NONICKNAMEGIVEN());
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
                send_message(cli.socket_fd, ERR_NICKNAMEINUSE(p.arg[0]));
            }
        }
        if (etat == true)
        {
            cli.nickName = p.arg[0];
            clients[cli.socket_fd] = cli;
            send_message(cli.socket_fd, ":irc.server.com NOTICE * :Nickname set, please send USER\r\n");
        }
    }
}

void Server::user(Client& cli, request &p)
{
    if (p.arg.size() < 4)
    {
        cli.count = 2;
        send_message(cli.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return;
    }

    cli.userName = p.arg[0];
    cli.hostName = p.arg[1];
    cli.serverName = p.arg[2];
    cli.realName = p.arg[3];
}
