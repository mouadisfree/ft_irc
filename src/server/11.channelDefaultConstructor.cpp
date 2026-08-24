#include "../../include/server.hpp"
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
