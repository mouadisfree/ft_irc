#pragma once

#define C_ESTABLISHING_CONNECTION 1
#define C_READ_REQUEST 2
#define C_RESPONSE_TO_REQUEST 3
#define C_CLOSE_CONNECTION 4

#define RPL_WELCOME(nickname)   (":irc.server.com 001 " + nickname + " :Welcome to the IRC Server, " + nickname + "!\r\n")
#define RPL_YOURHOST(client, servername) (":irc.server.com 002 " + client + " :Your host is " + servername + ", running version IRCd-1.0\r\n")
#define RPL_CREATED(client) (":irc.server.com 003 " + client + " :This server was created on January 1, 2022\r\n")
#define RPL_MYINFO(nickname, servername)    (":irc.server.com 004 " + nickname + " " + servername + " IRCd-1.0 <available user modes> <available channel modes> [<channel modes with a parameter>]\r\n")

#define ERR_PASSWDMISMATCH()  (std::string(":irc.server.com 464 * :Password incorrect\r\n"))
#define ERR_NEEDMOREPARAMS(cmd) (":irc.server.com 461 * " + cmd + " :Not enough parameters\r\n")
#define ERR_NICKNAMEINUSE(nick) (":irc.server.com 433 * " + nick + " :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN() (std::string(":irc.server.com 431 * :No nickname given\r\n"))
#define ERR_NOSUCHNICK(nick) (":irc.server.com 401 * " + nick + " :No such nick/channel\r\n")
#define ERR_NOTREGISTERED() (std::string(":irc.server.com 451 * :You have not registered\r\n"))
#define ERR_NOTEXTTOSEND() (std::string(":irc.server.com 412 * :No text to send\r\n"))
#define ERR_NORECIPIENT(cmd) (":irc.server.com 411 * :No recipient given (" + cmd + ")\r\n")
#define ERR_NOSUCHCHANNEL(channel) (":irc.server.com 403 * " + channel + " :No such channel\r\n")
#define RPL_ENDOFWHOIS(nickname) (":irc.server.com 318 " + nickname + " " + nickname + " :End of /WHOIS list\r\n")
#define RPL_ENDOFWHO(nickname, mask) (":irc.server.com 315 " + nickname + " " + mask + " :End of /WHO list\r\n")

#define KICKUSER(nick,user,channel, kicked) (":" + nick + "!" + user+ "@localhost KICK " + channel + " " + kicked + " :you have been kicked\r\n")

#define RPL_TOPIC(nick, channel, topic) (":irc.server.com 332 " + nick + " " + channel + " :" + topic + "\r\n")
#define ERR_CHANOPRIVSNEEDED(channel) (":irc.server.com 482 * " + channel + " :You're not channel operator\r\n")
#define RPL_JOINMSG(nick, username, channel) (":" + nick + "!" + username + "@localhost JOIN :" + channel + "\r\n")

#define ERR_INVALIDMODEPARAM(channel) (":irc.server.com 696 * " + channel + " :Invalid mode parameter\r\n")
#define ERR_INVALIDKEY(channel) (":irc.server.com 525 * " + channel + " :Invalid channel key\r\n")
#define ERR_BADCHANNELKEY(channel) (":irc.server.com 475 * " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_USERNOTINCHANNEL(nick,channel) (":irc.server.com 441 " + nick + " " + channel + " :They are not on that channel\r\n")
#define ERR_CHANNELISFULL(nick, channel) (":irc.server.com 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")
#define RPL_ALREAYREGISTRED(nick, channel) (":irc.server.com 462 " + nick + " " + channel + " :already on that channel\r\n")
#define ERR_INVITEONLYCHAN(nick, channel) (":irc.server.com 473 " + nick + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_KICKYOUSELF(nick) (":irc.server.com 488 " + nick + " :You can not kick yourself\r\n")
#define ERR_USERONCHANNEL(nick, channel) (":irc.server.com 443 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOTONCHANNEL(nick, channel) (":irc.server.com 442 " + nick + " " + channel + " :You're not on that channel\r\n")
#define RPL_INVITING(nick, channel, user) (":irc.server.com 341 " + nick + " " + user + " " + channel + "\r\n")
#define RPL_TOPICWHOTIME(nick, channel, admin) (":irc.server.com 333 " + nick + " " + channel + " " + admin + "\r\n")
