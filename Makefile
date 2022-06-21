I			:=	header/
O			:=	obj/

NAME		:=	webserv
SRC			:=	$(wildcard src/*.cpp)

OBJ			:=	$(SRC:%.cpp=$O%.o)

CXX			:=	c++

CXXFLAGS	+=	-I$I
CXXFLAGS	+=	-Wall -Wextra -Werror -std=c++98

LDFLAGS		+=	-I$I

RM			:=	/bin/rm -f
RMDIR		:=	/bin/rm -Rf

$O%.o: $S%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$O%_std.o: $S%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

all:
	@$(MAKE) $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $@

clean:
	$(RMDIR) $(wildcard $(NAME).dSYM)
	$(RMDIR) $O

fclean: clean
	$(RM) $(NAME)

re: fclean
	@$(MAKE)

.PHONY: all clean fclean re

-include $(OBJ:.o=.d)
