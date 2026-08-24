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

int		convertPortArgumentToPortNumber(const std::string &portArgument);
void	acceptNewClientConnection(std::map<int, Client> &clients, fd_set &totalfds, int server_fd);
void	parseRawLineIntoRequest(const std::string &line, request &req);

class Server {

	private:

		int			port;
		std::string	password;
		int 		server_fd;

		std::map<int, Client>	clients;
		std::map<std::string, Channel*>	channels;

	public :

		void	parseCommandLineArguments(std::string importedPort, std::string importedPassword);
		void	setupServerSocketAndStartListening();

		void	runServerEventLoop();
		void	disconnectAndRemoveClosedClients(std::vector<int> clientsToBeRemoved, fd_set &totalfds);
		void	removeClientFromChannels(Client &client);
		void	handleReadRequest(Client &client);

		void flushClient(Client &client);
		void queueMessage(int sockfd, const std::string &message);
		void sendMessageToOneClient(int sockfd, const std::string &message);
		int handlePassCommand(Client &client, request &p);
		int dispatchClientCommand(Client& cli, request&);
		void handleNickCommand(Client &client, request &p);
		void handleUserCommand(Client &client, request &p);

		std::string handleJoinCommand(Client &client, request &p);
		void createNewChannelAndJoinCreator(std::string &channel, Client &t, request&);
		void addClientToExistingChannel(std::string &channel, Client &t, request&);
		Channel	*findChannelByName(const std::string &name);
		void	sendChannelTopicReply(Client &t, Channel *chan);
		bool	isClientChannelOperator(Channel *chan, Client &cli);

		int findClientSocketByNickname(request& req);
		void sendPrivateMessageToClient(request& req, Client& cli, int client_dest);

		void	handlePartCommand(Client &client, request &p);
		void	handleQuitCommand(Client &client, request &p);
		std::string handleKickCommand(Client &client, request &p);
		std::string handleInviteCommand(Client &client, request &p);
		std::string handleTopicCommand(Client &client, request &p);
		void sendPrivateMessageToChannel(Client &cli, request &req);
		void handleModeCommand(Client &cli, request &req);
		void sendMessageToAllClientsExceptSender(int sockfd, const std::string &message);
		void sendMessageToChannelMembers(const std::string &message, std::string channel);

		bool isChannelMemberLimitReached(request& req, int *user);
		int validateChannelKeyAndJoinClient(Client& client, request& p, std::map<std::string, Channel *>::iterator it);
		bool isClientOperatorOfChannelInFirstArgument(request& req, Client& cli);
		bool isClientAdminOfChannelInSecondArgument(request& req, Client& cli);
};
