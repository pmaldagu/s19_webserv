SRCNAME 	=	srcs/main.cpp							\
				srcs/Config.cpp							\
				srcs/Location.cpp						\
				srcs/Server.cpp							\
				srcs/Webserv.cpp						\
				srcs/Client.cpp							\
				srcs/Request.cpp						\
				srcs/CGI.cpp							\

SRCS 		= ${SRCNAME}

OBJS 		= ${SRCS:.cpp=.o}

NAME		= webserver_exec

CXX 		= clang++
RM 			= rm -f
CXXFLAGS 	= -Wall -Wextra -Werror #-std=c++98

CGREEN		= \033[38;2;0;153;0m
CRED		= \033[0;31m
CYELLOW		= \033[0;33m
CGREY		= \033[38;2;128;128;128m
CEND		= \033[0m


.c.o:
		${CXX} ${CXXFLAGS} -c $< -o ${<:.c=.o}

${NAME}:	${OBJS}
		@echo
		@echo "$(CYELLOW)Compilation of containers$(CEND)"
		@echo "$(CGREY)"
		${CXX} -o ${NAME} ${OBJS}
		@echo "$(CEND)"
		@echo "$(CGREEN)Compilation done !$(CEND)"
		@echo

all:		${NAME}

clean:
		@echo
		@echo "$(CYELLOW)Deleting .o files$(CEND)"
		@echo "$(CGREY)"
		${RM} ${OBJS}
		@echo "$(CEND)"
		@echo "$(CGREEN)Deleting done !$(CEND)"
		@echo

fclean:		clean
		@echo "$(CYELLOW)Deleting .o files and containers$(CEND)"
		@echo "$(CGREY)"
		${RM} ${NAME}
		@echo "$(CEND)"
		@echo "$(CGREEN)Deleting done !$(CEND)"
		@echo

re: 		fclean all

c: 			all clean

.PHONY: 	clean fclean all re
