NAME = webserv

RESET		=	\033[0m
GREEN		=	\033[32m
YELLOW		=	\033[33m
BLUE		=	\033[34m
RED			=	\033[31m
UP			=	\033[A
CUT			=	\033[K

CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

CONFIG_PARSER_DIR = src/config_parser

SRC = src/main.cpp \
      $(CONFIG_PARSER_DIR)/Print.cpp \
      $(CONFIG_PARSER_DIR)/Tokenization.cpp \
      $(CONFIG_PARSER_DIR)/read.cpp \
      $(CONFIG_PARSER_DIR)/Parser.cpp \
      $(CONFIG_PARSER_DIR)/Config.cpp

OBJ_DIR = obj/
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(FLAGS) -o $(NAME) $(OBJ)

$(OBJ_DIR)%.o: src/%.cpp
	@mkdir -p $(dir $@)
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
