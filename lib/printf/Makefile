# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: davgalle <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/06 15:46:26 by davgalle          #+#    #+#              #
#    Updated: 2023/11/03 19:39:39 by davgalle         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftprintf.a

CC = gcc

CFLAGS = -Wall -Wextra -Werror

RM = rm -f

SRCS	=  ft_printf.c ft_printf_char.c ft_printf_numbers.c

LIB = ar rcs

OBJS = $(SRCS:.c=.o)

all:	$(NAME)

$(NAME):    $(OBJS)
	$(CC) -c $(CFLAGS) $(SRCS)
	$(LIB) $(NAME) $(OBJS)

clean:			
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re:	fclean all

.PHONY:	all clean fclean re
