# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/11 10:16:16 by moabdels          #+#    #+#              #
#    Updated: 2024/12/19 16:28:38 by moabdels         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
AR = ar
ARFLAGS = rcs
RM = rm -rf

VPATH =

SRC = ft_printf parser render_char render_int render_str writer format_utils
SRCS = $(addsuffix .c, $(SRC))

OBJS = $(SRCS:%.c=%.o)

LIBFT_PATH = ../libft
LIBFT = $(LIBFT_PATH)/libft.a

NAME = libftprintf.a
VPATH = ../libft

.o : .c
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

bonus: all

$(NAME): $(LIBFT) $(OBJS)
	cp $(LIBFT) $(NAME)
		$(AR) $(ARFLAGS) $(NAME) $(OBJS)

$(LIBFT):
	make -C $(LIBFT_PATH) all

clean:
	make -C $(LIBFT_PATH) clean
	$(RM) $(OBJS)

fclean: clean
	make -C $(LIBFT_PATH) fclean
	$(RM) $(NAME)

re: fclean all


.PHONY: all bonus clean fclean re libft
