NAME		= ircserv
CC			= c++  -g 
FLAGS		= -Wall -Wextra -Werror  -std=c++98 
OBJDIR 		= .obj

FILES		= 	Src/Server/Server \
				Src/Manage/Manage \
				Src/Client/Client \
				Src/Channel/Channel \
				Src/singleton/singleton \
				Src/main

HEADER		=	Src/Manage/Manage.hpp \
				Src/Server/Server.hpp \
				Src/Client/Client.hpp \
				Src/singleton/singleton.hpp \
				Src/Channel/Channel.hpp \

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	@$(CC) $(FLAGS) $(OBJ)   -o $(NAME) 
	@echo "🛰️  Server Ready!"

$(OBJDIR)/%.o: %.cpp $(HEADER)  Makefile
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -g -c $< -o $@ 

clean: 
	@rm -rf $(OBJDIR) $(OBJ)
	@echo  "🗑️   Deleting OBJS."

fclean: clean
	@rm -rf  $(NAME)
	@echo  "🗑️   Deleting $(NAME)."

re: fclean all

.PHONY: all clean fclean re