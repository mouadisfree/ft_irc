COMPILER = c++

CPPFLAGS = -I include/ -Wall -Werror -Wextra -std=c++98 -g #-fsanitize=address -g3

SRCS = src/main.cpp \
		src/server/parseCommandLineArguments.cpp \
		src/server/setupServerSocketAndStartListening.cpp \
		src/server/runServerEventLoop.cpp \
		src/server/acceptNewClientConnection.cpp \
		src/server/handleReadRequest.cpp \
		src/server/parseRawLineIntoRequest.cpp \
		src/commands/dispatchClientCommand.cpp \
		src/server/flushClient.cpp \
		src/server/disconnectAndRemoveClosedClients.cpp \
		src/commands/handleJoinCommand.cpp \
		src/commands/handleInviteCommand.cpp \
		src/commands/handleKickCommand.cpp \
		src/commands/handleModeCommand.cpp \
		src/commands/handleNickCommand.cpp \
		src/commands/handlePartCommand.cpp \
		src/commands/handlePassCommand.cpp \
		src/commands/handleQuitCommand.cpp \
		src/commands/handleTopicCommand.cpp \
		src/commands/handleUserCommand.cpp \
		src/commands/handlePRIVMSGCommand.cpp

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
