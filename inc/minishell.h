/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:04 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/25 14:29:47 by moabdels         ###   ########.fr       */
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
# define EX_FATAL_SIGNAL 128
# define EX_CMD_NOT_FOUND 127
# define EX_NOT_EXECUTED 126
# define EX_BAD_USAGE 258
# define WHITESPACE " \t\n\v\r\f"
# define UNEX_TOKEN "Syntax Error near unexpected token"

typedef struct s_env_list{
	char *name;
	char *value;
	char *data;
	struct s_env_list *next;
}	t_env_list;

typedef struct s_redirect
{
	t_token	token;
	char	*file;
	bool	flag;
	int		fd;
	struct	s_redirect	*left;
	struct	s_redirect	*right;
}	t_redirect;

typedef struct s_astree
{
	t_token		token;
	t_redirect	*redir_tree;
	int			fd_in;
	int			fd_out;
	char	**cmd;
	char	*prev_cmd;
	struct	s_astree	*left;
	struct	s_astree	*right;
} t_astree;




typedef enum e_token
{
	NOT,
	PIPE,
	HEREDOC,
	LEFT_PAREN,
	RIGHT_PAREN,
	AND,
	OR,
	APPEND,
	OUT,
	IN,
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

typedef struct s_signal_handler
{
	int			exit_code;
	int			executing;
	t_env_list	*env;
}	t_signal_handler;

extern	t_signal_handler	signal_handler;

t_env_list	*create_env_list(char **env);

int		check_list_dup(char *data, t_env_list *head);

void	add_env_var(char *data, t_env_list *head);
void	print_env(t_env_list *head);

bool	token_is_redir(t_token token);

char	*ft_expand(char *name, t_env_list *head);

#endif
