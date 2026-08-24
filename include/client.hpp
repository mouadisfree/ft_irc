#pragma once

#include "irc.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sstream>

#define BUFFER_SIZE 1024

struct Client {

	int	socket_fd;
	int step;

	std::string pass;
	std::string nickName;
	std::string hostName;
	std::string serverName;
	std::string userName;
	std::string realName;
	std::string buffer;
	std::string outBuffer;
	int count;
	std::vector<std::string> _channel;
  	bool authenticated;
	std::string quitReason;

	Client()
	{
		this->count = 0;
		this->step = C_ESTABLISHING_CONNECTION;
		this->authenticated = false;
		this->quitReason = "Client disconnected";
	}
};
