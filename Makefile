NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -I $(INCLUDES) -std=c++98

INCLUDES = includes
OBJ_DIR = objects

SRC_DIR = src
UTILS_DIR = $(SRC_DIR)/utils

SRCS = $(SRC_DIR)/main.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Client.cpp \
		$(UTILS_DIR)/utils.cpp

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
	nc 127.0.0.1 4444

spam:
	for i in 1 2 3 4 5; do nc -w 1 127.0.0.1 4444 & done

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 4444 senha123

client:
	c++ test_client.cpp -o client
