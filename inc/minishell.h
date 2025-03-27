/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:04 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/27 17:17:37 by moabdels         ###   ########.fr       */
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

typedef struct env_list{
	char *name;
	char *value;
	char *data;
	struct env_list *next;
}	env_list;

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

typedef struct s_redirection
{
	t_token					token;
	char					*file;
	bool					flg;
	int						fd;
	struct s_redirection	*left;
	struct s_redirection	*right;
}	t_redirection;

typedef struct	s_redir_node
{
	char				*prev_cmd;
	char				**cmd;
	int					precedence;
	int					fd[2];
	t_token				token;
	t_redirection		*redirections;
	struct s_redir_node	left;
	struct s_redir_node	right;
}	t_redir_node;

env_list	*create_env_list(char **env);

int	check_list_dup(char *data, env_list *head);

void	add_env_var(char *data, env_list *head);
void	print_env(env_list *head);

void	shell_init(char **envp, int *fd_in, int *fd_out);

char	*ft_expand(char *name, env_list *head);

#endif
