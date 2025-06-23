#include "../includes/minishell.h"

#define ERR_PIPE "Syntax error near unexpected token `|'"
#define ERR_AND "Syntax error near unexpected token `&&'"
#define ERR_OR "Syntax error near unexpected token `||'"

t_command	*create_command(void)
{
	t_command	*cmd;
	
	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->pipe_in = -1;
	cmd->pipe_out = -1;
	cmd->next_op = 0;
	cmd->is_subshell = 0;
	cmd->subshell = NULL;
	cmd->next = NULL;
	return (cmd);
}

// Helper function for add_arg when cmd->args is NULL
void	init_cmd_args(t_command *cmd, char *expanded_arg)
{
	cmd->args = (char **)malloc(2 * sizeof(char *));
	if (!cmd->args)
	{
		free(expanded_arg);
		return;
	}
	cmd->args[0] = expanded_arg;
	cmd->args[1] = NULL;
}

// Helper function for add_arg when cmd->args already exists
void	append_cmd_arg(t_command *cmd, char *expanded_arg)
{
	int		i;
	char	**new_args;

	i = 0;
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc((i + 2) * sizeof(char *));
	if (!new_args)
	{
		free(expanded_arg);
		return;
	}
	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = expanded_arg;
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

void	add_arg(t_command *cmd, char *arg)
{
	char	*expanded_arg;

	// Handle expansion (including wildcards) for arg
	expanded_arg = ft_strdup(arg);
	
	if (!cmd->args)
		init_cmd_args(cmd, expanded_arg);
	else
		append_cmd_arg(cmd, expanded_arg);
}

void	free_redirs(t_redir *redirs)
{
	t_redir	*temp;
	
	while (redirs)
	{
		temp = redirs;
		redirs = redirs->next;
		free(temp->file);
		free(temp);
	}
}

// Helper function to free command arguments
void	free_cmd_args(char **args)
{
	int	i;

	if (!args)
		return;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_commands(t_command *commands)
{
	t_command	*temp;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		if (temp->is_subshell && temp->subshell)
			free_commands(temp->subshell);
		
		free_cmd_args(temp->args);
		free_redirs(temp->redirs);
		free(temp);
	}
}

// paren_count will be 0 If we've found the matching closing parenthesis

t_token	*find_matching_paren(t_token *start)
{
	int		paren_count;
	t_token	*token;

	paren_count = 1;
	token = start->next;
	if (!token || start->type != TOKEN_LPAREN)
		return (NULL);
	while (token && paren_count > 0)
	{
		if (token->type == TOKEN_LPAREN)
			paren_count++;
		else if (token->type == TOKEN_RPAREN)
			paren_count--;
		if (paren_count == 0)
			return (token);
		token = token->next;
	}
	return (NULL);
}

t_token	*copy_tokens_section(t_token *start, t_token *end)
{
	t_token	*head;
	t_token	*token;
	t_token	*new_token;

	head = NULL;
	token = start->next;
	if (!token || token == end)
		return (NULL);
	while (token && token != end)
	{
		new_token = create_token(token->value, token->type);
		if (!new_token)
		{
			if (head)
				free_tokens(head);
			return (NULL);
		}
		add_to_token_list(&head, new_token);
		token = token->next;
	}
	return (head);
}

t_command	*parse_subshell(t_token *start, t_token *end)
{
	t_shell		temp_shell;
	t_token		*subshell_tokens;

	temp_shell.tokens = NULL;
	temp_shell.commands = NULL;
	temp_shell.exit_status = 0;
	temp_shell.running = 1;
	temp_shell.env = NULL;
	subshell_tokens = copy_tokens_section(start, end);
	if (!subshell_tokens && start->next != end)
	{
		ft_putendl_fd("Error: Failed to parse subshell", 2);
		return (NULL);
	}
	temp_shell.tokens = subshell_tokens;
	if (!parse_tokens(&temp_shell))
	{
		if (subshell_tokens)
			free_tokens(subshell_tokens);
		return (NULL);
	}
	if (subshell_tokens)
		free_tokens(subshell_tokens);
	return (temp_shell.commands);
}

void	parse_tokens_err(t_command *cmd_head, char *msg)
{
	ft_putstr_fd("Error: ", 2);
	ft_putendl_fd(msg, 2);
	free_commands(cmd_head);
}

// If wildcard expansion fails, use the original token value
// otherwise just add the argument as is

void	parse_tokens_word(char *value, t_command *cmd)
{
	char	**matches;
	int		i;

	if (!has_wildcards(value))
		return (add_arg(cmd, value));
	matches = expand_wildcards(value);
	if (!matches)
		return (add_arg(cmd, value));
	i = 0;
	while (matches[i])
	{
		add_arg(cmd, matches[i]);
		i++;
	}
	free_matches(matches);
}

// Helper function to handle parenthesis tokens
bool	handle_parenthesis(t_token **token_ptr, t_command *cmd, 
							t_command *cmd_head)
{
	t_token		*end_paren;
	t_command	*subshell_cmds;

	end_paren = find_matching_paren(*token_ptr);
	if (!end_paren)
	{
		parse_tokens_err(cmd_head, ERR_RPAREN);
		return (false);
	}
	subshell_cmds = parse_subshell(*token_ptr, end_paren);
	if (!subshell_cmds)
	{
		parse_tokens_err(cmd_head, "Failed to parse subshell");
		return (false);
	}
	cmd->is_subshell = 1;
	cmd->subshell = subshell_cmds;
	*token_ptr = end_paren;
	return (true);
}

// Helper function to handle word tokens with wildcards
// If wildcard expansion fails, use the original token value
bool	handle_wildcards_token(t_token *token, t_command *cmd)
{
	char	*expanded;

	expanded = handle_expansion(NULL, token->value);
	if (expanded)
	{
		add_arg(cmd, expanded);
		free(expanded);
		return (true);
	}
	parse_tokens_word(token->value, cmd);
	return (true);
}

// Helper function to handle word tokens
bool	handle_word_token(t_token *token, t_command *cmd)
{
	if (has_wildcards(token->value))
		return (handle_wildcards_token(token, cmd));
	add_arg(cmd, token->value);
	return (true);
}

// Helper function to handle pipe tokens
bool	handle_pipe_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_PIPE);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}

// Helper function to handle AND tokens
bool	handle_and_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_AND || 
		token->next->type == TOKEN_OR || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_AND);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next_op = 1;
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}

// Helper function to handle OR tokens
bool	handle_or_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_AND || 
		token->next->type == TOKEN_OR || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_OR);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next_op = 2;
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}

// Helper function to handle redirection tokens
bool	handle_redirection_token(t_token **token_ptr, t_command *cmd,
								t_command *cmd_head)
{
	t_token	*result;

	result = handle_redir(*token_ptr, cmd);
	if (!result)
	{
		parse_tokens_err(cmd_head, "Redirection error");
		return (false);
	}
	*token_ptr = result;
	return (true);
}

// Helper function to handle token based on its type
bool	process_token(t_token **token_ptr, t_command **cmd_ptr,
					t_command *cmd_head)
{
	t_token		*token;
	t_command	*cmd;

	token = *token_ptr;
	cmd = *cmd_ptr;
	if (token->type == TOKEN_WORD)
		return (handle_word_token(token, cmd));
	else if (token->type == TOKEN_PIPE)
		return (handle_pipe_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_AND)
		return (handle_and_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_OR)
		return (handle_or_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT ||
		token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC)
		return (handle_redirection_token(token_ptr, cmd, cmd_head));
	else if (token->type == TOKEN_LPAREN)
		return (handle_parenthesis(token_ptr, cmd, cmd_head));
	else if (token->type == TOKEN_RPAREN)
	{
		parse_tokens_err(cmd_head, ERR_RPAREN);
		return (false);
	}
	return (true);
}

bool	parse_tokens(t_shell *shell)
{
	t_token		*token;
	t_command	*cmd_head;
	t_command	*cmd;

	if (!shell->tokens)
		return (true);
	cmd_head = create_command();
	if (!cmd_head)
		return (false);
	cmd = cmd_head;
	token = shell->tokens;
	while (token)
	{
		if (!process_token(&token, &cmd, cmd_head))
			return (false);
		token = token->next;
	}
	shell->commands = cmd_head;
	return (true);
}
