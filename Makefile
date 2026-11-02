NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread
CPPFLAGS	= -Iinclude -MMD -MP

SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= main.c \
			  init.c \
			  simulation.c \
			  coder.c \
			  dongle.c \
			  scheduler.c \
			  monitor.c \
			  utils.c

OBJS		= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS		= $(OBJS:.o=.d)

all:		$(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c | $(OBJ_DIR)
			$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
			mkdir -p $@

# Rebuild with ThreadSanitizer and debug symbols to hunt data races.
debug:		CFLAGS += -g -fsanitize=thread
debug:		re

norm:
			norminette $(SRC_DIR) include

clean:
			rm -rf $(OBJ_DIR)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

-include $(DEPS)

.PHONY:		all debug norm clean fclean re
