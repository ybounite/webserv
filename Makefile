<<<<<<< HEAD

NAME = webserv

RESET		=	\033[0m
GREEN		=	\033[32m
YELLOW		=	\033[33m
BLUE		=	\033[34m
RED		=	\033[31m
UP		=	\033[A
CUT		=	\033[K

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

SRC = main.cpp 

OBJ_DIR = obj/
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "$(YELLOW)Compiling [$@]...$(RESET)"
	@$(CXX) $(FLAGS) -c $< -o $@
	@printf "$(UP)$(CUT)"
	@echo "$(GREEN)Finished [$@]$(RESET)"
	@printf "$(UP)$(CUT)"

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all
=======
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv
SRC = src/main.cpp src/Print.cpp src/Tokenization.cpp src/read.cpp src/Parser.cpp src/Config.cpp
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
>>>>>>> origin/sodahani
