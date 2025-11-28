FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = webserv

SRCS = Client.cpp Server.cpp main.cpp 

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	c++ $(FLAGS) -c $^ -o $@

$(NAME) : $(OBJS)
	c++ $(FLAGS) $(OBJS) -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re all

.SECONDARY: $(OBJS)