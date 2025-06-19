#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <errno.h>
# include <signal.h>
# include <dirent.h>
# include <string.h>
# include <termios.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

# define PROMPT "minishell> "
# define HEREDOC_PROMPT "> "
# define ERR_RPAREN "Syntax error near unexpected token `)'"

# define ERR_RPAREN "Syntax error near unexpected token `)'"

extern int	g_signal_code;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_LPAREN,    // Left parenthesis (
	TOKEN_RPAREN,    // Right parenthesis )
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_redir
{
	int				type;
	char			*file;
	struct s_redir	*next;
}	t_redir;

typedef struct s_command
{
	char			**args;
	t_redir			*redirs;
	int				pipe_in;
	int				pipe_out;
	int				next_op;  // 0 for none, 1 for &&, 2 for ||
	int             is_subshell; // Flag for subshell commands
	struct s_command	*subshell; // Pointer to subshell commands (if is_subshell is true)
	struct s_command	*next;
}	t_command;

typedef struct s_shell
{
	char		**env;
	t_token		*tokens;
	t_command	*commands;
	int			exit_status;
	int			running;
	char		*previous_cmd;
}	t_shell;

/* shell.c */
void	shell_init(t_shell *shell, char **env);
void	shell_loop(t_shell *shell);
void	shell_cleanup(t_shell *shell);

/* signals.c */
void	setup_signals(void);
void	handle_signals(int signum);
void	ignore_signals(void);

/* lexer.c */
bool		tokenize_input(t_shell *shell, char *input);
void	free_tokens(t_token *tokens);
t_token	*create_token(char *value, t_token_type type);
void    add_token(t_token **head, t_token *new_token);

/* redir.c */
t_token	*handle_redir(t_token *token, t_command *cmd);
void	add_redir(t_redir **head, int type, char *file);

/* parser.c */
bool		parse_tokens(t_shell *shell);
void	free_commands(t_command *commands);
t_token *find_matching_paren(t_token *start);
t_token *copy_tokens_section(t_token *start, t_token *end);
t_command *parse_subshell(t_token *start, t_token *end);

/* execution.c */
int		execute_commands(t_shell *shell);
int		execute_single_command(t_shell *shell, t_command *cmd);
int     execute_subshell(t_shell *shell, t_command *subshell_cmd);
char	*find_command_path(t_shell *shell, char *cmd);

/* builtins.c */
int		is_builtin(char *cmd);
int		execute_builtin(t_shell *shell, t_command *cmd);
int		builtin_echo(t_command *cmd);
char	*get_cd_path(t_shell *shell, char *arg);
int		builtin_cd(t_shell *shell, t_command *cmd);
int		builtin_pwd(void);
int		is_valid_var_name(char *name);
int		builtin_unset(t_shell *shell, t_command *cmd);
int		builtin_export(t_shell *shell, t_command *cmd);
void	process_export_arg(t_shell *shell, char *arg);
void	sort_env_vars(char **sorted_env, int env_size);
int		builtin_env(t_shell *shell);
int		builtin_exit(t_shell *shell, t_command *cmd);

/* redirections.c */
int             redirect_input(char *file);
int             redirect_output(char *file, int append);
int             handle_heredoc(t_shell *shell, t_redir *redir, char **temp_file);
int             setup_redirections(t_shell *shell, t_command *cmd);

/* env_utils.c */
char	**copy_env(char **env);
char	*get_env_value(t_shell *shell, const char *name);
void	set_env_var(t_shell *shell, const char *name, const char *value);
void	unset_env_var(t_shell *shell, const char *name);
void	add_env_var(t_shell *shell, char *new_env);
char	*get_env_name(char *env_var);

/* expansion.c */
char	*expand_variables(t_shell *shell, char *str, int in_quotes);
char	*handle_expansion(t_shell *shell, char *str);

/* wildcard expansion */
int		has_wildcards(const char *str);
char	**expand_wildcards(const char *pattern);
int		does_pattern_match(const char *pattern, const char *filename);
void	sort_matches(char **matches, int count);
char	*join_expanded_wildcards(char **matches);
void	free_matches(char **matches);

/* utils.c */
char	**split_args(char *str);
void	free_array(char **array);
int		count_array(char **array);
void	print_error(char *cmd, char *arg, char *message);
char	*ft_strjoin_free(char *s1, const char *s2);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_isdigit_str(char *str);

#endif 