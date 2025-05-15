NAME = libft.a

SRCS = ft_strlen.c \
	   ft_strdup.c \
	   ft_memcpy.c \
	   ft_strcmp.c \
	   ft_strjoin.c \
	   ft_split.c \
	   ft_substr.c \
	   ft_isspace.c \
	   ft_putstr_fd.c \
	   ft_isalnum.c \
	   ft_isalpha.c \
	   ft_isdigit.c \
	   ft_itoa.c \
	   ft_strchr.c \
	   ft_strncmp.c

OBJS = $(SRCS:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 