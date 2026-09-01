#pragma once

#include <string>
#include <vector>
#include <ctime>
#include "client.hpp"

class   Channel {

    public:
        std::string _name;
        std::string _pass;
        std::string _topic;
        std::string _password;

	    bool inviteOnly;
        bool    hasPassword;
        bool changeTopic;
        bool isLimit;
        int maxsize;

        std::vector<std::string> invitedUsers;
         std::string member_str;

        std::string topicSetter;
        long        topicTime;

        std::vector<Client *> _members;
        std::vector<Client *> admins;
        Client* admin;

        Channel()
            : _name(), _pass(), _topic(), _password(),
              inviteOnly(false), hasPassword(false), changeTopic(false),
              isLimit(false), maxsize(0), member_str(),
              topicSetter(), topicTime(0), admin(NULL)
        { }

        Channel(const std::string& channelName, Client *cl)
            : _name(channelName), _pass(), _topic(), _password(),
              inviteOnly(false), hasPassword(false), changeTopic(false),
              isLimit(false), maxsize(0), member_str(),
              topicSetter((cl != NULL) ? cl->nickName : ""),
              topicTime(static_cast<long>(std::time(NULL))), admin(NULL)
        {
            _members.push_back(cl);
        }

        ~Channel() { }
        void setTopicAndRecordSetter(std::string top, const std::string &setter);
        std::string getTopic();
};
