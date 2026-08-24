#include "../../include/server.hpp"
#include <ctime>

void Channel::setTopicAndRecordSetter(std::string top, const std::string &setter)
{
	this->_topic = top;
	this->topicSetter = setter;
	this->topicTime = static_cast<long>(std::time(NULL));
}
