NAME = minishell

SRCS_DIR = srcs
OBJS_DIR = objs
LIBFT_DIR = libft
INCLUDES_DIR = includes

LIBFT = $(LIBFT_DIR)/libft.a

SRCS = $(SRCS_DIR)/main.c \
	   $(SRCS_DIR)/lexer/tokenize.c \
	   $(SRCS_DIR)/lexer/token_utils.c \
	   $(SRCS_DIR)/parser/parser.c \
	   $(SRCS_DIR)/parser/parser_utils.c \
	   $(SRCS_DIR)/executor/executor.c \
	   $(SRCS_DIR)/executor/pipeline.c \
	   $(SRCS_DIR)/builtins/echo.c \
	   $(SRCS_DIR)/builtins/cd.c \
	   $(SRCS_DIR)/builtins/pwd.c \
	   $(SRCS_DIR)/builtins/export.c \
	   $(SRCS_DIR)/builtins/unset.c \
	   $(SRCS_DIR)/builtins/env.c \
	   $(SRCS_DIR)/builtins/exit.c \
	   $(SRCS_DIR)/utils/env_utils.c \
	   $(SRCS_DIR)/utils/signals.c \
	   $(SRCS_DIR)/utils/error.c \
	   $(SRCS_DIR)/utils/expand.c

OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I$(INCLUDES_DIR) -I$(LIBFT_DIR)
LIBS = -L$(LIBFT_DIR) -lft -lreadline

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(NAME)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)/lexer
	mkdir -p $(OBJS_DIR)/parser
	mkdir -p $(OBJS_DIR)/executor
	mkdir -p $(OBJS_DIR)/builtins
	mkdir -p $(OBJS_DIR)/utils

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJS_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 