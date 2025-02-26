NAME = ircserv
CC = c++
CFLAGS = -std=c++98 -fsanitize=address -g3 -Wall -Werror -Wextra
SRC = Client.cpp Server.cpp main.cpp Channel.cpp join.cpp mode.cpp invite.cpp topic.cpp kick.cpp
HEADERS = Client.hpp Server.hpp Channel.hpp

OSRC = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OSRC)
	@echo "compiling"
	@$(CC) $(CFLAGS) $(OSRC) -o $(NAME)

%.o : %.cpp $(HEADERS)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Removing Object Files.."
	@rm -rf $(OSRC)

fclean: clean
	@echo "Removing Exec"
	@rm -rf $(NAME)

re : fclean all

.PHONY : clean
