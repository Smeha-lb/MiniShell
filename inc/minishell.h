/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:04 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/11 13:59:24 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <errno.h>
# include <dirent.h>
# include "../lib/ft_printf/ft_printf.h"
# include "../lib/libft/libft.h"
# include "../inc/ft_memsafety.h"

# define OK 0

typedef struct s_env_list{
	char *name;
	char *value;
	char *data;
	struct env_list *next;
}	t_env_list;

typedef enum e_token
{
	PIPE,
	HEREDOC,
	LPR,
	RPR,
	AND,
	OR,
	APPEND,
	OUT,
	IN,
	NOT,
	END
}	t_token;

typedef struct s_minishell
{
	bool	is_tty;
	pid_t	shell_pid;
	pid_t	subshell_pid;
	char	*user_input;
	int		exit_status;
	t_env_list	*env;
}	t_minishell;

t_env_list	*create_env_list(char **env);

int	check_list_dup(char *data, t_env_list *head);

void	add_env_var(char *data, t_env_list *head);
void	print_env(t_env_list *head);

void	shell_init(char **envp, int *fd_in, int *fd_out);

char	*ft_expand(char *name, t_env_list *head);

#endif
