#include "../../include/server.hpp"

void Server::createNewChannelAndJoinCreator(std::string &channel, Client &t, request& p)
{
    (void)p;

    Channel *chan = new Channel(channel, &t);

    channels[channel] = chan;
    chan->changeTopic = true;
    chan->admin = &t;
    chan->admins.push_back(&t);
    t._channel.push_back(channel);

    sendMessageToOneClient(t.socket_fd,  ":" + t.nickName + "!" + t.userName + "@localhost JOIN :"+ channel + "\r\n");
    sendChannelTopicReply(t, chan);
    sendMessageToOneClient(t.socket_fd , ":irc.server.com 353 " + t.nickName + " = " + channel + " :@" + t.nickName + "\r\n");
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 366 " + t.nickName + " " + channel + " :End of /NAMES list.\r\n");
}
