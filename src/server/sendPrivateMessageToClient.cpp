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
