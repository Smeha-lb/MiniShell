/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <errno.h>
# include <string.h>
# include <termios.h>

# define PROMPT "minishell$ "
# define ERROR_PREFIX "minishell: "

/* Token types for lexer */
typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_END
}	t_token_type;

/* Token structure for lexical analysis */
typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

/* Redirection type */
typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC
}	t_redir_type;

/* Redirection structure */
typedef struct s_redir
{
	t_redir_type	type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

/* Command structure for AST */
typedef struct s_command
{
	char			**args;
	t_redir			*redirections;
	struct s_command	*next;
}	t_command;

/* Pipeline structure for AST */
typedef struct s_pipeline
{
	t_command			*commands;
	struct s_pipeline	*next;
}	t_pipeline;

/* Global variable for signal handling */
extern int	g_exit_status;

/* Main shell state structure */
typedef struct s_minishell
{
	char		**env;
	t_pipeline	*ast;
	int			last_exit_status;
	int			running;
	int			in_heredoc;
}	t_minishell;

/* Lexer functions */
t_token		*tokenize(char *input);
void		add_token(t_token **tokens, t_token_type type, char *value);
void		free_tokens(t_token *tokens);

/* Parser functions */
t_pipeline	*parse(t_token *tokens);
void		free_ast(t_pipeline *ast);
t_command	*parse_command(t_token **tokens);
t_redir		*parse_redirection(t_token **tokens);

/* Executor functions */
int			execute_ast(t_minishell *shell, t_pipeline *ast);
int			execute_pipeline(t_minishell *shell, t_command *commands);
int			execute_command(t_minishell *shell, t_command *cmd);
int			setup_redirections(t_redir *redirs, t_minishell *shell);

/* Built-in commands */
int			ft_echo(char **args);
int			ft_cd(t_minishell *shell, char **args);
int			ft_pwd(void);
int			ft_export(t_minishell *shell, char **args);
int			ft_unset(t_minishell *shell, char **args);
int			ft_env(t_minishell *shell);
int			ft_exit(t_minishell *shell, char **args);

/* Environment functions */
char		**init_env(char **envp);
char		*get_env_value(char **env, const char *name);
int			set_env_var(char ***env, const char *name, const char *value);
int			unset_env_var(char ***env, const char *name);

/* Signal handling */
void		setup_signals(int mode);
void		handle_signal(int sig);

/* Utilities */
void		print_error(const char *cmd, const char *msg);
char		*expand_variables(t_minishell *shell, char *str);
char		*remove_quotes(char *str);
void		free_array(char **array);
int			is_inside_single_quotes(char *str, int i);

#endif 