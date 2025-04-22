CC = cc
CFLAGS = -Wall -Werror -Wextra
NAME = philo
SRCS = main.c time.c valid.c routine.c routine_utils.c init.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean:clean
	rm -f $(NAME)

re:fclean all

.PHONY: all clean fclean re
