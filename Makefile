NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -I $(INCLUDES) -std=c++98

INCLUDES = includes
OBJ_DIR = objects

SRC_DIR = src
SERVER_DIR = $(SRC_DIR)/server
UTILS_DIR = $(SRC_DIR)/utils

SRCS = $(SRC_DIR)/main.cpp \
		$(SERVER_DIR)/Server.cpp \
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



run: all
	./$(NAME) 4444 senha123

#apagar dps
run2: all
	./$(NAME) aaaa senha123

#apagar dps
run3: all
	./$(NAME) -4444 senha123

#apagar dps
run4: all
	./$(NAME) 4444 senhaçç

val: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) 4444 senha123

re: fclean all