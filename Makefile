NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

HEADER		= codexion.h

SRCS		= main.c \
			  codexion.c \
			  compile.c \
			  dongles_init.c \
			  helpers.c \
			  heap_ops.c \
			  monitor.c \
			  utils.c

OBJS		= $(SRCS:.c=.o)

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o:		%.c $(HEADER)
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re