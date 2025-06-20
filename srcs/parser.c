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
		
		// Free subshell commands if any
		if (temp->is_subshell && temp->subshell)
			free_commands(temp->subshell);
		
		free_cmd_args(temp->args);
		free_redirs(temp->redirs);
		free(temp);
	}
}

t_token *find_matching_paren(t_token *start)
{
	int paren_count = 1;  // Start with 1 for the opening parenthesis
	t_token *token = start->next;
	
	if (!token || start->type != TOKEN_LPAREN)
		return NULL;
	
	while (token && paren_count > 0)
	{
		if (token->type == TOKEN_LPAREN)
			paren_count++;
		else if (token->type == TOKEN_RPAREN)
			paren_count--;
		
		// If we've found the matching closing parenthesis
		if (paren_count == 0)
			return token;
		
		token = token->next;
	}
	
	return NULL;  // No matching closing parenthesis found
}

t_token *copy_tokens_section(t_token *start, t_token *end)
{
	t_token *head = NULL;
	t_token *token = start->next;
	t_token *new_token;
	
	if (!token || token == end)
		return NULL;  // No tokens to copy or empty parentheses
	
	while (token && token != end)
	{
		// Create a copy of the current token
		new_token = create_token(token->value, token->type);
		if (!new_token)
		{
			// Failed to create token, free what we've copied so far
			if (head)
				free_tokens(head);
			return NULL;
		}
		
		// Add the new token to our copied list
		add_to_token_list(&head, new_token);
		
		// Move to next token
		token = token->next;
	}
	
	return head;
}

t_command *parse_subshell(t_token *start, t_token *end)
{
	t_shell temp_shell;
	t_token *subshell_tokens;
	
	// Initialize the temporary shell
	temp_shell.tokens = NULL;
	temp_shell.commands = NULL;
	temp_shell.exit_status = 0;
	temp_shell.running = 1;
	temp_shell.env = NULL;
	
	// Copy tokens from the subshell (skip the opening parenthesis)
	subshell_tokens = copy_tokens_section(start, end);
	if (!subshell_tokens && start->next != end)
	{
		// Error if we couldn't copy tokens and there should be tokens between parentheses
		ft_putendl_fd("Error: Failed to parse subshell", 2);
		return NULL;
	}
	
	temp_shell.tokens = subshell_tokens;
	
	// Parse the subshell tokens
	if (!parse_tokens(&temp_shell))
	{
		// Free subshell tokens if parsing fails
		if (subshell_tokens)
			free_tokens(subshell_tokens);
		return NULL;
	}
	
	// Free the tokens after parsing as they're no longer needed
	// (the commands structure now contains the parsed information)
	if (subshell_tokens)
		free_tokens(subshell_tokens);
	
	// Return the commands from parsing
	return temp_shell.commands;
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
	char **matches;
	
	if (!has_wildcards(value))
		return (add_arg(cmd, value));

	matches = expand_wildcards(value);
	if (!matches)
		return (add_arg(cmd, value));
	
	int i = 0;
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
	t_token *token = *token_ptr;
	t_token *closing_paren;

	// Find the matching closing parenthesis
	closing_paren = find_matching_paren(token);
	if (!closing_paren)
	{
		ft_putendl_fd("Error: Syntax error: unclosed parenthesis", 2);
		free_commands(cmd_head);
		return (false);
	}
	
	// Mark this command as a subshell
	cmd->is_subshell = 1;
	
	// Create subshell commands from the tokens inside parentheses
	cmd->subshell = parse_subshell(token, closing_paren);
	
	// Check if subshell parsing was successful
	if (!cmd->subshell)
	{
		ft_putendl_fd("Error: Failed to parse subshell commands", 2);
		free_commands(cmd_head);
		return (false);
	}
	
	// Skip to the closing parenthesis
	*token_ptr = closing_paren;
	return (true);
}

// Helper function to handle word tokens with wildcards
bool	handle_wildcards_token(t_token *token, t_command *cmd)
{
	char **matches;
	int i;

	matches = expand_wildcards(token->value);
	if (matches)
	{
		i = 0;
		while (matches[i])
		{
			add_arg(cmd, matches[i]);
			i++;
		}
		free_matches(matches);
	}
	else
	{
		// If wildcard expansion fails, use the original token value
		add_arg(cmd, token->value);
	}
	return (true);
}

// Helper function to handle word tokens
bool	handle_word_token(t_token *token, t_command *cmd)
{
	if (has_wildcards(token->value))
		return (handle_wildcards_token(token, cmd));
	
	// No wildcards, just add the argument as is
	add_arg(cmd, token->value);
	return (true);
}

// Helper function to handle pipe tokens
bool	handle_pipe_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command *cmd = *cmd_ptr;

	if (!token->next)
	{
		ft_putendl_fd("Error: Syntax error near unexpected token `|'", 2);
		free_commands(cmd_head);
		return (false);
	}
	cmd->next = create_command();
	*cmd_ptr = cmd->next;
	return (true);
}

// Helper function to handle AND tokens
bool	handle_and_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command *cmd = *cmd_ptr;

	if (!token->next)
	{
		ft_putendl_fd("Error: Syntax error near unexpected token `&&'", 2);
		free_commands(cmd_head);
		return (false);
	}
	cmd->next_op = 1;  // Mark for AND operator
	cmd->next = create_command();
	*cmd_ptr = cmd->next;
	return (true);
}

// Helper function to handle OR tokens
bool	handle_or_token(t_token *token, t_command **cmd_ptr,
						t_command *cmd_head)
{
	t_command *cmd = *cmd_ptr;

	if (!token->next)
	{
		ft_putendl_fd("Error: Syntax error near unexpected token `||'", 2);
		free_commands(cmd_head);
		return (false);
	}
	cmd->next_op = 2;  // Mark for OR operator
	cmd->next = create_command();
	*cmd_ptr = cmd->next;
	return (true);
}

// Helper function to handle redirection tokens
bool	handle_redirection_token(t_token **token_ptr, t_command *cmd,
								t_command *cmd_head)
{
	t_token *token = *token_ptr;
	t_token *next;

	next = handle_redir(token, cmd);
	if (!next)
	{
		free_commands(cmd_head);
		return (false);
	}
	*token_ptr = next;
	return (true);
}

// Helper function to handle token based on its type
bool	process_token(t_token **token_ptr, t_command **cmd_ptr,
					t_command *cmd_head)
{
	t_token *token = *token_ptr;
	t_command *cmd = *cmd_ptr;
	
	if (!token)
		return true;
		
	if (token->type == TOKEN_LPAREN)
	{
		if (!handle_parenthesis(token_ptr, cmd, cmd_head))
			return false;
	}
	else if (token->type == TOKEN_RPAREN)
	{
		ft_putendl_fd(ERR_RPAREN, 2);
		free_commands(cmd_head);
		return false;
	}
	else if (token->type == TOKEN_WORD)
	{
		if (!handle_word_token(token, cmd))
			return false;
	}
	else if (token->type == TOKEN_PIPE)
	{
		if (!handle_pipe_token(token, cmd_ptr, cmd_head))
			return false;
	}
	else if (token->type == TOKEN_AND)
	{
		if (!handle_and_token(token, cmd_ptr, cmd_head))
			return false;
	}
	else if (token->type == TOKEN_OR)
	{
		if (!handle_or_token(token, cmd_ptr, cmd_head))
			return false;
	}
	else if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT ||
			token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC)
	{
		if (!handle_redirection_token(token_ptr, cmd, cmd_head))
			return false;
	}
	
	return true;
}

bool	parse_tokens(t_shell *shell)
{
	t_token		*token;
	t_command	*cmd;
	t_command	*cmd_head;

	token = shell->tokens;
	if (!token)
		return (true);
	cmd = create_command();
	cmd_head = cmd;
	while (token)
	{
		if (!process_token(&token, &cmd, cmd_head))
			return (false);
		token = token->next;
	}
	shell->commands = cmd_head;
	return (true);
}
