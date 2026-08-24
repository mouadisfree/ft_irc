COMPILER = c++

CPPFLAGS = -I include/ -Wall -Werror -Wextra -std=c++98 -g #-fsanitize=address -g3

SRCS = src/main.cpp \
		$(wildcard src/server/*.cpp) \
		$(wildcard src/commands/*.cpp)

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
