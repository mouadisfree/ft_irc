#include "../../include/server.hpp"

void Server::queueMessage(int sockfd, const std::string &message)
{
    std::map<int, Client>::iterator it = clients.find(sockfd);

    if (it == clients.end())
        return;

    it->second.outBuffer += message;
}

void Server::sendMessageToOneClient(int sockfd, const std::string& message)
{
    queueMessage(sockfd, message);
}

int Server::findClientSocketByNickname(request& req) {
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

int Server::dispatchClientCommand(Client& cli, request& req)
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
                sendMessageToOneClient(cli.socket_fd, ERR_NOTREGISTERED());
                return (cli.count);
            }
        }
    }

    if (req.cmd == "PRIVMSG")
    {
        if (req.arg.empty() || req.arg[0].empty())
        {
            sendMessageToOneClient(cli.socket_fd, ERR_NORECIPIENT(req.cmd));
            return (cli.count);
        }

        if (req.arg[0][0] == '#')
        {
            sendPrivateMessageToChannel(cli, req);
        }
        else
        {
            int client_dest;

            client_dest = findClientSocketByNickname(req);
            sendPrivateMessageToClient(req, cli, client_dest);
        }
    }
    else if (req.cmd == "CAP")
    {
        if (!req.arg.empty() && (req.arg[0] == "LS" || req.arg[0] == "LIST"))
            sendMessageToOneClient(cli.socket_fd, ":irc.server.com CAP * " + req.arg[0] + " :\r\n");
        else if (!req.arg.empty() && req.arg[0] == "REQ")
        {
            std::string wanted = (req.arg.size() > 1) ? req.arg[1] : std::string("");
            sendMessageToOneClient(cli.socket_fd, ":irc.server.com CAP * NAK :" + wanted + "\r\n");
        }
    }
    else if (req.cmd == "WHOIS")
    {
        sendMessageToOneClient(cli.socket_fd, RPL_ENDOFWHOIS(cli.nickName));
    }
    else if (req.cmd == "WHO")
    {
        std::string mask = req.arg.empty() ? std::string("*") : req.arg[0];
        sendMessageToOneClient(cli.socket_fd, RPL_ENDOFWHO(cli.nickName, mask));
    }
    else if (req.cmd == "PING")
    {
        std::string token = req.arg.empty() ? std::string("irc.server.com") : req.arg[0];
        sendMessageToOneClient(cli.socket_fd, ":irc.server.com PONG irc.server.com :" + token + "\r\n");
    }
    else if (req.cmd == "MODE")
    {
        handleModeCommand(cli, req);
    }
    else if (req.cmd == "join" || req.cmd == "JOIN")
    {
        handleJoinCommand(cli, req);
    }
    else if (req.cmd == "INVITE")
    {
        handleInviteCommand(cli, req);
    }
    else if (req.cmd == "KICK")
    {
        handleKickCommand(cli, req);
    }
    else if (req.cmd == "PART" || req.cmd == "part")
    {
        handlePartCommand(cli, req);
    }
    else if (req.cmd == "QUIT" || req.cmd == "quit")
    {
        handleQuitCommand(cli, req);
        return (cli.count);
    }
    else if (req.cmd == "TOPIC")
    {
        handleTopicCommand(cli, req);
    }
    else if ((req.cmd == "PASS" || req.cmd == "pass") && cli.count == 0)
    {
        cli.count = 1;
        handlePassCommand(cli, req);
    }
    else if ((req.cmd == "NICK" || req.cmd == "nick") && cli.count == 1)
    {
        cli.count = 2;
        handleNickCommand(cli, req);
    }
    else if ((req.cmd == "USER" || req.cmd == "user") && cli.count == 2)
    {
        cli.count = 3;
        handleUserCommand(cli, req);
    }
    else
        std::cout << req.cmd << " not a command" << std::endl;
    return (cli.count);
}
