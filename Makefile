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
		shell2.c \
		signals.c \
		lexer.c \
		lexer_utils.c \
		lexer_utils2.c \
		lexer_quotes.c \
		lexer_quotes2.c \
		lexer_expansion.c \
		lexer_expansion2.c \
		lexer_word.c \
		lexer_word2.c \
		lexer_word3.c \
		lexer_word4.c \
		redir.c \
		parser_core.c \
		parser_command.c \
		parser_args.c \
		parser_args2.c \
		parser_args3.c \
		parser_token_handlers.c \
		parser_subshell.c \
		parser_cleanup.c \
		execution_path.c \
		execution_cmd.c \
		execution_cmd2.c \
		execution_pipes.c \
		execution_pipes2.c \
		execution_pipes3.c \
		execution_pipeline.c \
		execution_pipeline2.c \
		execution_pipeline3.c \
		execution_pipeline_utils.c \
		execution_pipeline_utils2.c \
		execution_pipeline_utils3.c \
		execution_subshell.c \
		execution_subshell2.c \
		execution_logic.c \
		execution_logic2.c \
		execution_logic3.c \
		execution_logic4.c \
		execution_logic5.c \
		execution_logic6.c \
		execution_commands.c \
		builtins.c \
		builtins2.c \
		builtins3.c \
		builtins4.c \
		builtins5.c \
		builtins6.c \
		builtins7.c \
		redirections.c \
		redirections_apply.c \
		redirections_heredoc.c \
		redirections_heredoc2.c \
		redirections_cleanup.c \
		env_utils.c \
		env_utils2.c \
		utils.c \
		utils2.c \
		expansion.c \
		expansion2.c \
		expansion_core.c \
		expansion_quotes.c \
		expansion_vars.c \
		wildcard_expansion.c \
		wildcard_expansion2.c \
		wildcard.c \
		wildcard2.c \
		wildcard3.c \
		wildcard4.c \
		wildcard5.c \
		wildcard6.c

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