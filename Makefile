NAME = ircserv
CPP = c++

RED = \033[0;31m
GREEN = \033[0;32m
RESET = \033[0m

SRC_DIR = ./IRC
HEADER_DIR = ./Headers
OBJ_DIR = ./OBJ

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

HEADERS = $(wildcard $(HEADER_DIR)/*.hpp)
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(OBJ_DIR) $(NAME)
	@echo "${GREEN}--------------- server is ready ---------------${RESET}"

$(OBJ_DIR):
	@mkdir -p $@

$(NAME): $(OBJECTS)
	@$(CPP) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "${RED}--------------- Object files removed. ---------------${RESET}"

fclean: clean
	@rm -f $(NAME)
	@echo "${RED}--------------- $(NAME) removed. ---------------${RESET}"

re: fclean all
