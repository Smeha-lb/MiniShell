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
# define ERR_PIPE "Syntax error near unexpected token `|'"
# define ERR_AND "Syntax error near unexpected token `&&'"
# define ERR_OR "Syntax error near unexpected token `||'"

# define ERR_RPAREN "Syntax error near unexpected token `)'"

extern int	g_signal_code;

typedef struct s_match_data
{
	char	**matches;
	char	*dir_part;
	char	*file_part;
	DIR		*dir;
	int		capacity;
	int		match_count;
}	t_match_data;

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
	TOKEN_LPAREN,
	TOKEN_RPAREN,
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

// next_op takes 0 for none, 1 for &&, 2 for ||
typedef struct s_command
{
	char				**args;
	t_redir				*redirs;
	int					pipe_in;
	int					pipe_out;
	int					next_op;
	int					is_subshell;
	struct s_command	*subshell;
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

typedef struct s_pipeline_child
{
	t_shell		*shell;
	t_command	*cmd;
	int			**pipes;
	int			i;
	int			cmd_count;
}	t_pipeline_child;

typedef struct s_pipeline
{
	t_shell		*shell;
	t_command	*start_cmd;
	pid_t		*pids;
	int			**pipes;
	int			cmd_count;
	int			is_nested;
}	t_pipeline;

/* shell.c */
void	shell_init(t_shell *shell, char **env);
void	shell_loop(t_shell *shell);
void	shell_cleanup(t_shell *shell);
void	clean_tokens(t_shell *shell);

/* signals.c */
void	setup_signals(void);
void	handle_signals(int signum);
void	ignore_signals(void);

/* lexer.c */
bool	tokenize_input(t_shell *shell, char *input);

/* lexer_utils.c */
void	free_tokens(t_token *tokens);
t_token	*create_token(char *value, t_token_type type);
void	add_to_token_list(t_token **head, t_token *new_token);
int		handle_special_char(t_shell *shell, char *input, int *i);

/* lexer_quotes.c */
int		handle_quotes(char *input, int *i, char **word, int *j, t_shell *shell);
char	*extract_quoted_content(char *input, int start, int end);
char	*process_quoted_content(t_shell *shell, char *content, char quote_type);

/* lexer_expansion.c */
typedef struct s_var_data
{
	char	*var_name;
	char	*var_value;
	size_t	value_len;
	int		var_name_len;
}	t_var_data;

void	add_expanded_tokens(t_shell *shell, char *expanded_value);
char	*get_variable_value(t_shell *shell, char *var_name);
int		is_var_delimiter(char c, char next_c);
int		get_var_name_len(char *input);
int		handle_simple_variable_part2(t_shell *shell, char *input, int *i, int var_name_len);
int		handle_simple_variable(t_shell *shell, char *input, int *i);
int		process_var_expansion(t_var_data *data, char **word, int *j);
int		process_variable_expansion_part2(char *input, int *i, char **word, int *j, t_shell *shell, int var_name_len);
int		process_variable_expansion(char *input, int *i, char **word, int *j, t_shell *shell);

/* lexer_word.c */
int		is_word_delimiter(char c, char next_c);
int		calculate_word_buffer_size(char *input, int i);
int		process_word_character(char *input, int *i, char **word, int *j, t_shell *shell);
int		handle_complex_word(char *input, int *i, t_shell *shell);
int		extract_word(char *input, int *i, t_shell *shell);
int		handle_word(char *input, int *i, t_shell *shell);

/* redir.c */
t_token	*handle_redir(t_token *token, t_command *cmd);
void	add_redir(t_redir **head, int type, char *file);

/* parser_core.c */
bool	parse_tokens(t_shell *shell);
void	parse_tokens_err(t_command *cmd_head, char *msg);
bool	process_token(t_token **token_ptr, t_command **cmd_ptr, t_command *cmd_head);

/* parser_command.c */
t_command	*create_command(void);
bool	handle_pipe_token(t_token *token, t_command **cmd_ptr, t_command *cmd_head);
bool	handle_and_token(t_token *token, t_command **cmd_ptr, t_command *cmd_head);
bool	handle_or_token(t_token *token, t_command **cmd_ptr, t_command *cmd_head);

/* parser_args.c */
void	init_cmd_args(t_command *cmd, char *expanded_arg);
void	append_cmd_arg(t_command *cmd, char *expanded_arg);
void	add_arg(t_command *cmd, char *arg);
void	parse_tokens_word(char *value, t_command *cmd);
bool	handle_wildcards_token(t_token *token, t_command *cmd);

/* parser_token_handlers.c */
bool	handle_word_token(t_token *token, t_command *cmd);
bool	handle_redirection_token(t_token **token_ptr, t_command *cmd, t_command *cmd_head);
bool	handle_parenthesis(t_token **token_ptr, t_command *cmd, t_command *cmd_head);

/* parser_subshell.c */
t_token	*find_matching_paren(t_token *start);
t_token	*copy_tokens_section(t_token *start, t_token *end);
t_command	*parse_subshell(t_token *start, t_token *end);

/* parser_cleanup.c */
void	free_redirs(t_redir *redirs);
void	free_cmd_args(char **args);
void	free_commands(t_command *commands);

/* execution_commands.c */
int		execute_commands(t_shell *shell);
int		execute_command_with_redirects(t_shell *shell, t_command **cmd,
		int *exit_status);

/* execution_path.c */
char	*check_direct_path(char *cmd);
char	*search_in_path(char *cmd, char *path_env);
char	*find_command_path(t_shell *shell, char *cmd);
int		is_parent_builtin(char *cmd_name);
int		execute_single_command(t_shell *shell, t_command *cmd);

/* execution_cmd.c */
void	setup_child_redirections(t_command *cmd);
void	handle_child_process(t_shell *shell, t_command *cmd, char *path);
int		fork_external_command(t_shell *shell, t_command *cmd, char *path);
int		wait_for_external_command(pid_t pid);
int		execute_external_command(t_shell *shell, t_command *cmd);

/* execution_pipes.c */
int		create_pipes(t_command *commands);
void	close_pipes(t_command *commands);
int		**allocate_pipes(int cmd_count, pid_t **pids);
int		setup_pipes(int **pipes, int cmd_count, pid_t *pids);
void	setup_child_pipes(int **pipes, int i, int cmd_count);

/* execution_pipeline.c */
int		check_nested_minishell(t_command *start_cmd);
int		count_pipeline_cmds(t_command *start_cmd);
void	execute_child_cmd(t_shell *shell, t_command *cmd);
int		handle_pipeline_child(t_pipeline_child *pc);
int		wait_for_pipeline(pid_t *pids, int cmd_count, int is_nested);

/* execution_pipeline_utils.c */
void	free_pipes(int **pipes, int cmd_count);
void	init_pipeline(t_pipeline *pipeline, t_shell *shell, t_command *start_cmd);
void	fork_pipeline_processes(t_pipeline *pipeline, t_command *start_cmd);
int		execute_pipeline(t_shell *shell, t_command *start_cmd);
int		wait_for_all(pid_t last_pid);

/* execution_subshell.c */
void	init_subshell(t_shell *subshell, t_shell *shell, t_command *subshell_cmd);
int		handle_parent_process(pid_t pid, int *exit_status);
int		execute_subshell(t_shell *shell, t_command *subshell_cmd);
int		handle_subshell(t_shell *shell, t_command *cmd, int *exit_status);
int		setup_builtin_redirects(t_shell *shell, t_command *cmd, int *in, int *out);

/* execution_logic.c */
void	handle_and_operator(t_command **cmd, int exit_status);
void	handle_or_operator(t_command **cmd, int exit_status);
void	handle_next_command(t_command **cmd, int exit_status);
int		handle_redirection_failure(int stdin_backup, int stdout_backup,
		int *exit_status);
int		execute_current_command(t_shell *shell, t_command **cmd, int *exit_status);

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
int		setup_redirections(t_shell *shell, t_command *cmd);

/* redirections_apply.c */
int		redirect_input(char *file);
int		redirect_output(char *file, int append);
int		apply_all_redirections(t_command *cmd, char **heredoc_tempfiles);

/* redirections_heredoc.c */
int		handle_heredoc(t_shell *shell, t_redir *redir, char **temp_file);
int		apply_heredoc_redirection(char *temp_file);
char	*create_heredoc_tempfile(void);
void	setup_heredoc_signals(void);
int		count_heredocs(t_command *cmd);

/* redirections_cleanup.c */
void	cleanup_heredoc_tempfiles(char **temp_files, int count);
char	**init_heredoc_tempfiles(int count);
int		process_heredocs(t_shell *shell, t_command *cmd, char **tempfiles);

/* env_utils.c */
char	**copy_env(char **env);
char	*get_env_value(t_shell *shell, const char *name);
void	set_env_var(t_shell *shell, const char *name, const char *value);
void	unset_env_var(t_shell *shell, const char *name);
void	add_env_var(t_shell *shell, char *new_env);
char	*get_env_name(char *env_var);

/* expansion.c */
char	*expand_variables(t_shell *shell, char *str, int in_quotes);
void	copy_variable(t_shell *shell, char *str, int *i, char *expanded);
char	*expand_variables(t_shell *shell, char *str, int in_quotes);
char	*allocate_expanded_string(t_shell *shell, char *str);
void	process_char(char *str, int *i, char *expanded, int *j);
int		calculate_expanded_length(t_shell *shell, char *str);

/* wildcard_expansion.c */
char	*handle_expansion(t_shell *shell, char *str);
char	*process_quoted_segment(t_shell *shell, char *str, int *i, char quote);
char	*process_unquoted_segment(t_shell *shell, char *str, int *i);
int		is_quote(char c);
char	*init_result(void);
int		has_wildcards(const char *str);
char	**expand_wildcards(const char *pattern);
int		does_pattern_match(const char *pattern, const char *filename);
void	sort_matches(char **matches, int count);
char	*join_expanded_wildcards(char **matches);
int		is_in_char_class(char c, const char *class_str);
void	split_path(const char *path, char **dir_part, char **file_part);
char	**init_matches(int capacity);
void	swap_matches(char **matches, int i, int j);
int		add_matching_entry(t_match_data *data, char *entry_name);
void	finalize_matches(char **matches, int match_count,
		const char *pattern);
void	free_array(char **array);
int		count_array(char **array);
void	print_error(char *cmd, char *arg, char *message);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_isdigit_str(char *str);
void	free_matches(char **matches);
char	**handle_no_wildcards(const char *pattern);
char	**handle_dir_error(char *dir_part, char *file_part);
char	**process_wildcard_matches(t_match_data *data, const char *pattern);
char	*join_path(const char *dir, const char *file);

#endif