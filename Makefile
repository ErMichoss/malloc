NAME = libft_malloc_$(HOSTTYPE).so
CC = gcc
CFLAGS = -Wall -Werror -Wextra -fPIC -g -Iincl
RM = rm -f

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

SRC = src/malloc.c src/free.c src/realloc.c src/memory_zone.c src/aux.c \
      src/globals.c src/blocks.c src/auxTwo.c src/memory_zone_bonus.c

OBJS = $(SRC:.c=.o)

FT_PRINTF_DIR = lib/printf
FT_PRINTF = $(FT_PRINTF_DIR)/libftprintf.a

TEST_EXEC = test_malloc_app
TEST_SRC = test/test_malloc.c

all: $(NAME)

$(NAME): $(OBJS) $(FT_PRINTF)
	$(CC) -shared -o $(NAME) $(OBJS) -L$(FT_PRINTF_DIR) -lftprintf

libft_malloc.so: $(NAME)
	@ln -sf $(NAME) libft_malloc.so

test: libft_malloc.so $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) -L. -lft_malloc -Wl,-rpath,'$$ORIGIN' -o $(TEST_EXEC)

$(FT_PRINTF):
	@make -C $(FT_PRINTF_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TEST_EXEC)
	@make clean -C $(FT_PRINTF_DIR)

fclean: clean
	$(RM) $(NAME) libft_malloc.so
	@make fclean -C $(FT_PRINTF_DIR)

re: fclean all

.PHONY: all clean fclean re $(FT_PRINTF) libft_malloc.so test