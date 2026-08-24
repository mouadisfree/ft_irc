#include "../../headers/server.hpp"
#include <ctime>

Channel::Channel() { }

Channel::Channel(std::string &channelName, Client *cl)
{
	_name = channelName;
	_pass = "";
	_password = "";
	_members.push_back(cl);
	_topic = "";
	inviteOnly = false;
	hasPassword = false;
	changeTopic = false;
	isLimit = false;
	maxsize = 0;
	member_str = "";
	topicSetter = (cl != NULL) ? cl->nickName : "";
	topicTime = static_cast<long>(std::time(NULL));
}

Channel::~Channel(){}

void Channel::set_topic(std::string top, const std::string &setter)
{
	this->_topic = top;
	this->topicSetter = setter;
	this->topicTime = static_cast<long>(std::time(NULL));
}
std::string Channel::get_topic()
{
	return (this->_topic);
}
