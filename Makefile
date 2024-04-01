NAME = ircserv

CPP = c++

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

HEADER_P = IRC/Server.hpp IRC/Channel.hpp IRC/Database.hpp IRC/Client.hpp

# Directories
SRCDIR = IRC
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