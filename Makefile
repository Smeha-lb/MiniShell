<<<<<<< HEAD
<<<<<<< HEAD
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/18 13:06:22 by moabdels          #+#    #+#              #
#    Updated: 2025/03/19 15:16:11 by moabdels         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= minishell

CC				:= cc
SRC_DIR			:= src/
OBJ_DIR			:= obj/
CFLAGS			:= -g -O3 -Wall -Werror -Wextra
FSANITIZE		:= -fsanitize=address -fsanitize=leak -fsanitize=undefined
RM				:= rm -f

INC				:= inc/
LIB				:= lib/
PRINTF_DIR		:= $(LIB)ft_printf/
PRINTF			:= $(PRINTF_DIR)libftprintf.a
HEADER			:= -I$(INC) -I$(LIBFT_DIR)

SRC_FILES		= main env_utils

SRC				= $(addprefix $(SRC_DIR), $(addsuffix .c, $(SRC_FILES)))
OBJ				= $(addprefix $(OBJ_DIR), $(addsuffix .o, $(SRC_FILES)))
DEPS			= $(addprefix $(OBJ_DIR), $(addsuffix .d, $(SRC_FILES)))
INCS			= $(addprefix $(INCLUDE), $(addsuffix .h, $(INC_FILES)))

OBJF			= .cache_exists

all:	makelibs
		@$(MAKE) $(NAME)

makelibs:
	@$(MAKE) -C $(PRINTF_DIR)

# if you want to test for leaks make sure to uncomment the value of FSANITIZE

-include	${DEPS}
$(NAME):	$(OBJ)
			@$(CC) $(CFLAGS) $(FSANITIZE) $(OBJ) $(PRINTF) $(LIBS) -o $(NAME)
			@echo "⚙️$(CC) $(CFLAGS) $(FSANITIZE) $(OBJ) $(PRINTF) $(LIBS) -o $(NAME)"
			@echo "Project Compiled Successfully!📀"

bonus:
			@$(MAKE) all

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(INCS) | $(OBJF)
				$(CC) $(CFLAGS) -MMD -c $< -o $@

$(OBJF):
				@mkdir -p $(OBJ_DIR)

$(PRINTF):
	@make -C $(PRINTF_DIR)
	@echo "🟢 ft_printf compiled!"

clean:
		@make clean -s -C $(PRINTF_DIR)
		@echo "🔵 ft_printf objects and files cleaned!"
		$(RM) -r $(OBJ_DIR)
		@echo "🔵 Project Object Files Cleaned!"

re:		fclean makelibs
		@$(MAKE)
		@echo "🟢 Cleaned and Rebuilt Everything!"

norm:
		@norminette $(SRC) $(INCLUDE) | grep -v Norme -B1 || true

.PHONY: all test bonus lib_clean clean fclean re norm
=======
=======
>>>>>>> 40893fbd894187c9bc8b97e773e97fd7d53b9158
# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Target executable
TARGET = MiniShell

# Source files
SRCS = main.c env_utils.c

INCLUDES = mini_shell.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS)

# Clean up build files and executable
fclean: clean
	rm -f $(TARGET)

# Rebuild the project
re: fclean all

# Phony targets
<<<<<<< HEAD
.PHONY: all clean fclean re
>>>>>>> 0f15d9e (Added export function)
=======
.PHONY: all clean fclean re
>>>>>>> 40893fbd894187c9bc8b97e773e97fd7d53b9158
