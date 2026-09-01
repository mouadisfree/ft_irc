#include "../../include/server.hpp"
#include <ctime>

void Channel::setTopicAndRecordSetter(std::string top, const std::string &setter)
{
	this->_topic = top;
	this->topicSetter = setter;
	this->topicTime = static_cast<long>(std::time(NULL));
}

std::string Channel::getTopic()
{
	return (this->_topic);
}

std::string Server::handleTopicCommand(Client &client, request &p)
{
    if (p.arg.empty())
    {
        sendMessageToOneClient(client.socket_fd, ERR_NEEDMOREPARAMS(p.cmd));
        return ("");
    }

    Channel *chan = findChannelByName(p.arg[0]);

    if (chan == NULL)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOSUCHCHANNEL(p.arg[0]));
        return ("");
    }

    bool isMember = false;
    for (std::vector<Client *>::iterator itt = chan->_members.begin(); itt != chan->_members.end(); ++itt)
    {
        if (*itt == &client)
        {
            isMember = true;
            break;
        }
    }

    if (!isMember)
    {
        sendMessageToOneClient(client.socket_fd, ERR_NOTONCHANNEL(client.nickName, p.arg[0]));
        return ("");
    }

    if (p.arg.size() < 2)
    {
        sendChannelTopicReply(client, chan);
        return ("");
    }

    if (chan->changeTopic && !isClientChannelOperator(chan, client))
    {
        sendMessageToOneClient(client.socket_fd, ERR_CHANOPRIVSNEEDED(p.arg[0]));
        return ("");
    }

    chan->setTopicAndRecordSetter(p.arg[1], client.nickName);

    sendMessageToChannelMembers(":" + client.nickName + "!" + client.userName
        + "@localhost TOPIC " + chan->_name + " :" + chan->getTopic() + "\r\n", p.arg[0]);
    return ("");
}
