NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -I $(INCLUDES) -std=c++98

INCLUDES = includes
OBJ_DIR = objects

SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/utils
COMMANDS_DIR = $(SRC_DIR)/commands

SRCS = $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Client.cpp \
		$(SRC_DIR)/Auth.cpp \
		$(SRC_DIR)/Channel.cpp \
		$(SRC_DIR)/Commands.cpp \
		$(UTILS_DIR)/utils.cpp \
		$(COMMANDS_DIR)/join.cpp \
		$(COMMANDS_DIR)/kick.cpp \
		$(COMMANDS_DIR)/mode.cpp \
		$(COMMANDS_DIR)/nick.cpp \
		$(COMMANDS_DIR)/part.cpp \
		$(COMMANDS_DIR)/ping.cpp \
		$(COMMANDS_DIR)/privmsg.cpp \
		$(COMMANDS_DIR)/quit.cpp \
		$(COMMANDS_DIR)/topic.cpp \
		$(COMMANDS_DIR)/invite.cpp 

OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: all
	./$(NAME) 4444 senha123

kill:
	killall -9 $(NAME) || true

nc:
	nc -C 127.0.0.1 4444

nc-manual:
	nc 127.0.0.1 4444

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes ./$(NAME) 4444 senha123