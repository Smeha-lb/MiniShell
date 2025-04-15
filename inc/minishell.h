/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:04 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/24 15:05:36 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../lib/libft/libft.h"
# include "../lib/ft_printf/ft_printf.h"
# include "../inc/ft_memsafety.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>



typedef enum e_token
{
	WORD = 0,
	ERR = -1,
	END = -2,
	ASSIGN,
	PIPE,
	HEREDOC,
	LEFT_PAREN,
	RIGHT_PAREN,
	AND,
	OR,
	APPEND,
	OUT,
	IN,
}	t_token;

typedef struct s_minishell
{
	bool	is_executing;
	int		exit_status;
	t_env_list	*env;
}	t_minishell;

env_list	*create_env_list(char **env);

int	check_list_dup(char *data, env_list *head);

void	add_env_var(char *data, env_list *head);
void	print_env(env_list *head);

void	shell_init(char **envp, int *fd_in, int *fd_out);

char	*ft_expand(char *name, env_list *head);

#endif
