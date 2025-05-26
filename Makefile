NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -rf

SRCS_DIR = srcs/
INCL_DIR = includes/
OBJS_DIR = objs/

LIBFT_DIR = libft/
LIBFT = $(LIBFT_DIR)libft.a

SRCS = main.c \
	   shell.c \
	   signals.c \
	   lexer.c \
	   parser.c \
	   execution.c \
	   builtins.c \
	   redirections.c \
	   env_utils.c \
	   utils.c \
	   expansion.c \
	   wildcard.c

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.c=.o))

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline

$(OBJS_DIR)%.o: $(SRCS_DIR)%.c
	@mkdir -p $(OBJS_DIR)
	@$(CC) $(CFLAGS) -I$(INCL_DIR) -c $< -o $@

clean:
	@make clean -C $(LIBFT_DIR)
	@$(RM) $(OBJS_DIR)

fclean: clean
	@make fclean -C $(LIBFT_DIR)
	@$(RM) $(NAME)

re: fclean all

bonus: all

.PHONY: all clean fclean re bonus 