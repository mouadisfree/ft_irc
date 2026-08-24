#pragma once

#include <iostream>
#include <csignal>

#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <stdexcept>
#include <limits.h>
#include <vector>
#include <netinet/in.h>
#include <cerrno>
#include "client.hpp"
#include "irc.hpp"
#include "channel.hpp"

struct request {

	std::string cmd;
	std::vector<std::string> arg;

	request () {

		cmd = "";
	}
};

class Server {

	private:

		int			port;
		std::string	password;
		int 		server_fd;
		
		std::map<int, Client>	clients;
		std::map<std::string, Channel*>	channels;

	public :

		void	parseArgs(std::string importedPort, std::string importedPassword);
		void	setupListener();

		void	awaitingTraffic();
		void	clearClients(std::vector<int> clientsToBeRemoved, fd_set &totalfds);
		void	removeClientFromChannels(Client &client);
		void	handleReadRequest(Client &client);

		void flushClient(Client &client);
		void queueMessage(int sockfd, const std::string &message);
		void send_message(int sockfd, const std::string &message);
		int pass(Client &client, request &p);
		int getAuthentified(Client& cli, request&);
		void Nick(Client& Client, request &p);
		void user(Client& client, request &p);

		std::string join(Client &client, request &p);
		void createChannel(std::string &channel, Client &t, request&);
		void joinChannel(std::string &channel, Client &t, request&);
		std::string join_message(std::string channel, int fd);
		bool is_admin(request& req, Client&);
		Channel	*getChannel(const std::string &name);
		void	sendTopicReply(Client &t, Channel *chan);
		bool	isOperator(Channel *chan, Client &cli);

		int searchForDestination(request& req);
		void sendMessageToClient(request& req, Client& cli, int client_dest);

		void	part(Client &client, request &p);
		void	quit(Client &client, request &p);
		std::string kick(Client &client, request &p);
		std::string invite(Client &client, request &p);
		std::string Topic(Client &client, request &p);
		void sendMSGToChannel(Client &, request &);
		void Mode(Client&, request&);
		void send_all_member(int sockfd, const std::string &message);
		void send_just_member(const std::string &message, std::string);

		bool checkLimits(request& req, int *user);
		int joinClient(Client& client, request& p, std::map<std::string, Channel *>::iterator it);
		bool isAdmin(request& req, Client& cli);
};
