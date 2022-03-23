SRCNAME 	=	srcs/main.cpp							\
				srcs/Config.cpp							\
				srcs/Location.cpp						\
				srcs/Server.cpp							\
				srcs/Webserv.cpp						\
				srcs/Client.cpp							\
				srcs/Request.cpp						\
				srcs/CGI.cpp							\
				srcs/utils.cpp

SRCS 		= ${SRCNAME}

OBJS 		= ${SRCS:.cpp=.o}

NAME		= webserv

CXX 		= clang++
RM 			= rm -f
CXXFLAGS 	= -g -Wall -Wextra -Werror -std=c++98


#---------------COLORS---------------
CGREEN		= \033[38;2;0;153;0m
CRED		= \033[0;31m
CYELLOW		= \033[0;33m
CGREY		= \033[38;2;128;128;128m
CEND		= \033[0m


#---------------RULES---------------
#.c.o:
%.o: %.c
		@${CXX} ${CXXFLAGS} -c $< -o ${<:.c=.o}

${NAME}:	${OBJS}
			@echo
			@${CXX} $(CXXFLAGS) ${OBJS} -o ${NAME}
			@echo "$(CGREEN)Compilation done !$(CEND)"
			@echo

all:		${NAME}

clean:
		@echo
		@echo "$(CYELLOW)Clean .o files$(CEND)"
		@echo "$(CGREY)"
		${RM} ${OBJS}
		@echo "$(CEND)"
		@echo "$(CGREEN)Cleaning done !$(CEND)"
		@echo

fclean:		clean
		@echo "$(CYELLOW)Clean executable.$(CEND)"
		@echo "$(CGREY)"
		${RM} ${NAME}
		@echo "$(CEND)"
		@echo "$(CGREEN)Cleaning done !$(CEND)"
		@echo

re: 		fclean all

c: 			all clean

.PHONY: 	clean fclean all re
