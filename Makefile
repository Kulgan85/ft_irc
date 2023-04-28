SRC_FILES		=	main.cpp \
					Server.cpp \
					Client.cpp \
					commands.cpp \
					PrivMsg.cpp \
					SHA1.cpp \
					PingPong.cpp \
					Channel.cpp \
					ChannelCommands.cpp \
					Utils.cpp

SRC_DIR			=	srcs/

SRCS			=	$(addprefix $(SRC_DIR), $(SRC_FILES))

OBJ_FILES		=	$(SRC_FILES:.cpp=.o)

OBJ_DIR			=	objects/

OBJS			=	$(addprefix $(OBJ_DIR), $(OBJ_FILES))

INCS			=	-Iincludes/


PLAIN			=	\033[0m
RED				=	\033[3;91m
CYAN			=	\033[36m

CC				=	c++
RM				=	rm -rf
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++98

NAME			=	ircserv

all:			$(NAME)

$(NAME):		$(OBJS)
				@$(CC) $(CFLAGS) -o $@ $^
				@echo "$(RED)ft_irc has compiled!$(PLAIN)"

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp | $(OBJ_DIR)
				@$(CC) $(CXXFLAGS) -c $(INCS) -o $@ $<

$(OBJ_DIR):
				@mkdir $@

clean:
				@$(RM) $(OBJ_DIR)

fclean:			clean
				@$(RM) $(NAME)

re:			fclean $(OBJ_DIR) all

.PHONY:			all clean fclean re
