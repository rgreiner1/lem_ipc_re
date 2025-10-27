SRCS    =	lem_ipc.c \
			lem_ipc_init.c \
			lem_ipc_game.c \
			
OBJS    =   ${SRCS:.c=.o}
RM      =   rm -f
CFLAGS  =   -Wall -Wextra -Werror -lpthread -lrt
NAME	= 	lem-ipc

.c.o:
		gcc ${CFLAGS} -c $< -o $@
${NAME}:	${OBJS}
	gcc ${CFLAGS} ${OBJS} -o ${NAME}
all:	${NAME}
clean:
	${RM} ${OBJS}
fclean: clean
	${RM} ${NAME}
re:	fclean all

.PHONY: all clean fclean re%