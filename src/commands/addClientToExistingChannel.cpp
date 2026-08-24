#include "../../include/server.hpp"

void Server::addClientToExistingChannel(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = findChannelByName(channel);

    if (chan == NULL)
        return;

    chan->_members.push_back(&t);
    t._channel.push_back(channel);

    chan->member_str = ":irc.server.com 353 " + t.nickName + " = " + channel + " :";
    for (std::vector<Client*>::iterator it = chan->_members.begin(); it != chan->_members.end(); ++it)
    {
        if (isClientChannelOperator(chan, **it))
            chan->member_str += "@";
        chan->member_str += (*it)->nickName + " ";
    }
    chan->member_str += "\r\n";

    sendMessageToOneClient(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendChannelTopicReply(t, chan);
    sendMessageToOneClient(t.socket_fd , chan->member_str);
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}
