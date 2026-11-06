
NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
CPPFLAGS = -I.

SRCS = src/main.c \
       src/parse/parser.c \
       src/init/init_simulation.c \
       src/utils/destroy_simulation.c \
       src/simulation/run_simulation.c \
       src/simulation/coder_routine.c \
       src/utils/time_utils.c \
       src/simulation/dongle.c \
       src/simulation/simulation_state.c \
       src/simulation/monitor_routine.c \
       src/simulation/start_barrier.c \
       src/scheduler/heap.c \
       src/scheduler/request.c \
       src/scheduler/grant_request.c \
       src/scheduler/wait_request.c \
       src/scheduler/heap_utils.c


OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
