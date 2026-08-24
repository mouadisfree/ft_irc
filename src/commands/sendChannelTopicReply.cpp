#include "../../include/server.hpp"
#include <sstream>

void Server::sendChannelTopicReply(Client &t, Channel *chan)
{
    if (chan->getTopic().empty())
    {
        sendMessageToOneClient(t.socket_fd, ":irc.server.com 331 " + t.nickName + " "
            + chan->_name + " :No topic is set\r\n");
        return;
    }

    std::stringstream when;
    when << chan->topicTime;

    sendMessageToOneClient(t.socket_fd, RPL_TOPIC(t.nickName, chan->_name, chan->getTopic()));
    sendMessageToOneClient(t.socket_fd, ":irc.server.com 333 " + t.nickName + " " + chan->_name
        + " " + chan->topicSetter + " " + when.str() + "\r\n");
}
