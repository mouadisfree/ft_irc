#pragma once

#include <string>
#include "server.hpp"

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

        Channel();
        Channel(std::string& channelName, Client *cl);
        ~Channel();
        void setTopicAndRecordSetter(std::string top, const std::string &setter);
        std::string getTopic();
};
