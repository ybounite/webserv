CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv
SRC = src/main.cpp src/Print.cpp src/Tokenization.cpp 
OBJ = $(SRC:.cpp=.o)
all: $(NAME)
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all
BRANCH = sodahani
p:
	@read -p "Commit message: " msg; \
	git add . && git commit -m "$$msg" && git push origin $(BRANCH)
