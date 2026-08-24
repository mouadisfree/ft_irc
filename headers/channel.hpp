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
        // A channel can have several pending invitations at once, so this
        // must be a list and not a single nickname.
        std::vector<std::string> invitedUsers;
         std::string member_str;

        // Reply 333 must report WHO set the topic and WHEN, otherwise clients
        // display an empty setter and the epoch (1 Jan 1970).
        std::string topicSetter;
        long        topicTime;

        std::vector<Client *> _members;
        std::vector<Client *> admins;
        Client* admin;

        Channel();
        Channel(std::string& channelName, Client *cl);
        ~Channel();
        void update_onlinemembers();
        void set_topic(std::string top, const std::string &setter);
        std::string get_topic();
};
