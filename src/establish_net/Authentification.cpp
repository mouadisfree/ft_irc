
#include "../../headers/server.hpp"
#include <cstddef>
#include <linux/limits.h>
#include <sstream>
#include <string>

int Server::searchForDestination(request& req) {
    if (req.arg.empty())
        return -1;

    std::map<int, Client>::iterator it;
    for (it = clients.begin(); it != clients.end(); ++it) {
        if (it->second.nickName == req.arg[0]) {
            return it->second.socket_fd;
        }
    }
    return -1;
}

void Server::sendMessageToClient(request& req, Client& cli, int client_dest)
{
    std::string msg;
    std::string str;

    if (client_dest == -1)
    {
        send_message(cli.socket_fd, ERR_NOSUCHNICK(req.arg[0]));
        return;
    }

    if (req.arg.size() < 2)
    {
        send_message(cli.socket_fd, ERR_NOTEXTTOSEND());
        return;
    }

    str = req.arg[1];

    msg = ":" + cli.nickName + "!~" + cli.userName + "@localhost PRIVMSG " + req.arg[0] + " :" + str + "\r\n";
    queueMessage(client_dest, msg);
}

int Server::getAuthentified(Client& cli, request& req)
{
    static const std::string needsRegistration[] = {
        "PRIVMSG", "WHOIS", "MODE", "JOIN", "join", "INVITE", "KICK", "TOPIC", "PART"
    };
    static const size_t needsRegistrationCount = sizeof(needsRegistration) / sizeof(needsRegistration[0]);

    if (cli.count != 3)
    {
        for (size_t i = 0; i < needsRegistrationCount; i++)
        {
            if (req.cmd == needsRegistration[i])
            {
                send_message(cli.socket_fd, ERR_NOTREGISTERED());
                return (cli.count);
            }
        }
    }

    if (req.cmd == "PRIVMSG")
    {
        if (req.arg.empty() || req.arg[0].empty())
        {
            send_message(cli.socket_fd, ERR_NORECIPIENT(req.cmd));
            return (cli.count);
        }

        if (req.arg[0][0] == '#')
        {
            sendMSGToChannel(cli, req);
        }
        else
        {
            int client_dest;

            client_dest = searchForDestination(req);
            sendMessageToClient(req, cli, client_dest);
        }
    }
    else if (req.cmd == "CAP")
    {
        if (!req.arg.empty() && (req.arg[0] == "LS" || req.arg[0] == "LIST"))
            send_message(cli.socket_fd, ":irc.server.com CAP * " + req.arg[0] + " :\r\n");
        else if (!req.arg.empty() && req.arg[0] == "REQ")
        {
            std::string wanted = (req.arg.size() > 1) ? req.arg[1] : std::string("");
            send_message(cli.socket_fd, ":irc.server.com CAP * NAK :" + wanted + "\r\n");
        }
    }
    else if (req.cmd == "WHOIS")
    {
        send_message(cli.socket_fd, RPL_ENDOFWHOIS(cli.nickName));
    }
    else if (req.cmd == "PING")
    {
        std::string token = req.arg.empty() ? std::string("irc.server.com") : req.arg[0];
        send_message(cli.socket_fd, ":irc.server.com PONG irc.server.com :" + token + "\r\n");
    }
    else if (req.cmd == "MODE")
    {
        Mode(cli,req);
    }
    else if (req.cmd == "join" || req.cmd == "JOIN")
    {
        join(cli, req);
    }
    else if (req.cmd == "INVITE")
    {
        invite(cli, req);
    }
    else if (req.cmd == "KICK")
    {
        kick(cli, req);
    }
    else if (req.cmd == "PART" || req.cmd == "part")
    {
        part(cli, req);
    }
    else if (req.cmd == "QUIT" || req.cmd == "quit")
    {
        quit(cli, req);
        return (cli.count);
    }
    else if (req.cmd == "TOPIC")
    {
        Topic(cli, req);
    }
    else if ((req.cmd == "PASS" || req.cmd == "pass") && cli.count == 0)
    {
        cli.count = 1;
        pass(cli, req);
    }
    else if ((req.cmd == "NICK" || req.cmd == "nick") && cli.count == 1)
    {
        cli.count = 2;
        Nick(cli, req);
    }
    else if ((req.cmd == "USER" || req.cmd == "user") && cli.count == 2)
    {
        cli.count = 3;
        user(cli, req);
    }
    else
        std::cout << req.cmd << " not a command" << std::endl;
    return (cli.count);
}
