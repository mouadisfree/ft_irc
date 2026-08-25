COMPILER = c++

CPPFLAGS = -I include/ -Wall -Werror -Wextra -std=c++98 -g #-fsanitize=address -g3

SRCS = src/main.cpp \
		src/server/1.parseCommandLineArguments.cpp \
		src/server/2.setupServerSocketAndStartListening.cpp \
		src/server/3.runServerEventLoop.cpp \
		src/server/4.acceptNewClientConnection.cpp \
		src/server/5.handleReadRequest.cpp \
		src/server/6.parseRawLineIntoRequest.cpp \
		src/server/7.dispatchClientCommand.cpp \
		src/server/8.flushClient.cpp \
		src/server/9.disconnectAndRemoveClosedClients.cpp \
		src/server/10.queueMessage.cpp \
		src/server/11.channelDefaultConstructor.cpp \
		src/commands/1.handleJoinCommand.cpp \
		src/commands/2.handleInviteCommand.cpp \
		src/commands/3.handleKickCommand.cpp \
		src/commands/4.handleModeCommand.cpp \
		src/commands/5.handleNickCommand.cpp \
		src/commands/6.handlePartCommand.cpp \
		src/commands/7.handlePassCommand.cpp \
		src/commands/8.handleQuitCommand.cpp \
		src/commands/9.handleTopicCommand.cpp \
		src/commands/10.handleUserCommand.cpp \
		src/commands/11.handlePRIVMSGCommand.cpp

HEADERS = include/server.hpp include/client.hpp include/channel.hpp include/irc.hpp

NAME = ircserv

all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	$(COMPILER) $(CPPFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re
