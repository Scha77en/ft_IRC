NAME = ircserv

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address

HEADER_P = Parse/Server.hpp Parse/Channel.hpp Parse/Database.hpp Parse/Client.hpp

# Directories
SRCDIR = Parse
BINDIR = bin

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BINDIR)/%.o)

all:$(NAME)

$(NAME): $(OBJECTS)
	@$(CPP) $(CPPFLAGS) $^ -o $@

# Object file rule
$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(HEADER_P)
	@$(CPP) $(CPPFLAGS) -c $< -o $@


fclean: clean
	@rm -f $(NAME)
clean:
	@rm -f $(BINDIR)/*.o

re: fclean all