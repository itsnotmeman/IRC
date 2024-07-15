NAME		= ircserv

ifeq ($(shell uname), Linux)
	CC		= g++
else
	CC		= c++
endif

CFLAGS		= -Wall -Wextra -Werror -std=c++98 -pedantic -Wfatal-errors #-g3 -fsanitize=address #-Wno-unused
#CFLAGS		= -Wall -Wextra -Werror -std=c++98 #-g3 -fsanitize=address #-Wno-unused

RM			= rm -f

SRC			= 	srcs/main.cpp \
				srcs/Server.cpp \
				srcs/ServerNetworking.cpp \
				srcs/ServerParse.cpp \
				srcs/ServerCmd.cpp \
				srcs/ServerCmd1.cpp \
				srcs/ServerCmd2.cpp \
				srcs/ServerCmd3.cpp \
				srcs/ServerCmd4.cpp \
				srcs/MessageCenter.cpp \
				srcs/User.cpp \
				srcs/Channel.cpp \
				srcs/Utility.cpp
				
HEADERS		= 	srcs/Server.hpp \
				srcs/MessageCenter.hpp \
				srcs/User.hpp \
				srcs/Channel.hpp \
				srcs/Utility.hpp

OBJ			= $(SRC:.cpp=.o)

%.o : %.cpp	$(HEADERS)
			$(CC) $(CFLAGS) -c $< -o $@

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

all:		$(NAME)

clean:
			$(RM) $(OBJ)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

leaks:		$(NAME)
			leaks -atExit -- ./$(NAME)

.PHONY:		all clean fclean re 
