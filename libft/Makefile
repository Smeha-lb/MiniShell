NAME = libft.a

CC = cc
CFLAGS = -Wall -Wextra -Werror
AR = ar rcs
RM = rm -f

SRCS = ft_isdigit.c \
	   ft_strlen.c \
	   ft_strdup.c \
	   ft_strchr.c \
	   ft_strjoin.c \
	   ft_substr.c \
	   ft_split.c \
	   ft_putchar_fd.c \
	   ft_putstr_fd.c \
	   ft_putendl_fd.c \
	   ft_atoi.c \
	   ft_isalpha.c \
	   ft_strcmp.c \
	   ft_isdigit_str.c \
	   ft_strjoin_free.c \
	   ft_isalnum.c \
	   ft_strlcpy.c \
	   ft_itoa.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re 