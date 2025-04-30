# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/18 13:06:22 by moabdels          #+#    #+#              #
#    Updated: 2025/05/05 17:09:32 by moabdels         ###   ########.fr        #
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
LIBFT_DIR		:= $(LIB)libft/
PRINTF			:= $(PRINTF_DIR)libftprintf.a
LIBFT			:= $(LIBFT_DIR)libft.a
HEADER			:= -I$(INC) -I$(LIBFT_DIR)

# ? replace this with any required libraries present in path

LIBS			:= -lreadline -lhistory

SRC_FILES		= main env_utils err init lexing pwd_builtin tokens \
					# ft_atoi get_next_line_bonus get_next_line_utils_bonus

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
			@$(CC) $(CFLAGS) $(FSANITIZE) $(OBJ) $(PRINTF) $(LIBFT) $(LIBS) -o $(NAME)
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

fclean: clean
		@echo "🧹 Cleaning Executables and Libs 🧹"
		$(RM) $(NAME)
		$(RM) (LIBFT_DIR)libft.a

re:		fclean makelibs
		@$(MAKE)
		@echo "🟢 Cleaned and Rebuilt Everything!"

norm:
		@norminette $(SRC) $(INCLUDE) | grep -v Norme -B1 || true

.PHONY: all test bonus lib_clean clean fclean re norm
