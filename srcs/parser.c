#include "../includes/minishell.h"

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
	cmd->next_op = 0;  // Initialize next_op to 0 (none)
	cmd->is_subshell = 0;  // Initialize is_subshell to 0 (false)
	cmd->subshell = NULL;  // Initialize subshell to NULL
	cmd->next = NULL;
	return (cmd);
}

void	add_redir(t_redir **head, int type, char *file)
{
	t_redir	*new_redir;
	t_redir	*current;

	new_redir = (t_redir *)malloc(sizeof(t_redir));
	if (!new_redir)
		return;
	new_redir->type = type;
	new_redir->file = ft_strdup(file);
	new_redir->next = NULL;
	if (!*head)
	{
		*head = new_redir;
		return;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

void	add_arg(t_command *cmd, char *arg)
{
	int		i;
	char	**new_args;
	char	*expanded_arg;

	// Handle expansion (including wildcards) for arg
	expanded_arg = ft_strdup(arg);
	
	if (!cmd->args)
	{
		cmd->args = (char **)malloc(2 * sizeof(char *));
		if (!cmd->args)
		{
			free(expanded_arg);
			return;
		}
		cmd->args[0] = expanded_arg;
		cmd->args[1] = NULL;
		return;
	}
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

void	free_commands(t_command *commands)
{
	t_command	*temp;
	int			i;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		
		// Free subshell commands if any
		if (temp->is_subshell && temp->subshell)
			free_commands(temp->subshell);
			
		if (temp->args)
		{
			i = 0;
			while (temp->args[i])
			{
				free(temp->args[i]);
				i++;
			}
			free(temp->args);
		}
		free_redirs(temp->redirs);
		free(temp);
	}
}

t_token	*handle_redir(t_token *token, t_command *cmd)
{
	t_token_type	type;
	t_token			*next_token;

	type = token->type;
	next_token = token->next;
	if (!next_token || next_token->type != TOKEN_WORD)
	{
		ft_putendl_fd("Error: Syntax error near redirection", 2);
		return (NULL);
	}
	if (type == TOKEN_REDIR_IN)
		add_redir(&cmd->redirs, TOKEN_REDIR_IN, next_token->value);
	else if (type == TOKEN_REDIR_OUT)
		add_redir(&cmd->redirs, TOKEN_REDIR_OUT, next_token->value);
	else if (type == TOKEN_REDIR_APPEND)
		add_redir(&cmd->redirs, TOKEN_REDIR_APPEND, next_token->value);
	else if (type == TOKEN_HEREDOC)
		add_redir(&cmd->redirs, TOKEN_HEREDOC, next_token->value);
	return (next_token);
}

// Function to find the matching closing parenthesis
t_token *find_matching_paren(t_token *start)
{
	int paren_count = 1;  // Start with 1 for the opening parenthesis
	t_token *token = start->next;
	
	while (token && paren_count > 0)
	{
		if (token->type == TOKEN_LPAREN)
			paren_count++;
		else if (token->type == TOKEN_RPAREN)
			paren_count--;
		
		if (paren_count == 0)
			return token;
			
		token = token->next;
	}
	
	return NULL;  // No matching closing parenthesis found
}

// Function to create a new tokens list from a subshell section
t_token *copy_tokens_section(t_token *start, t_token *end)
{
	t_token *head = NULL;
	t_token *token = start->next;  // Skip the opening parenthesis
	
	while (token && token != end)  // Stop before the closing parenthesis
	{
		add_token(&head, create_token(token->value, token->type));
		token = token->next;
	}
	
	return head;
}

// Function to parse a subshell command
t_command *parse_subshell(t_token *start, t_token *end)
{
	t_shell temp_shell;
	t_token *subshell_tokens;
	
	// Initialize temporary shell
	temp_shell.tokens = NULL;
	temp_shell.commands = NULL;
	
	// Create a copy of the tokens between parentheses
	subshell_tokens = copy_tokens_section(start, end);
	temp_shell.tokens = subshell_tokens;
	
	// Parse the subshell tokens
	if (parse_tokens(&temp_shell) != 0)
	{
		// Clean up on error
		if (subshell_tokens)
			free_tokens(subshell_tokens);
		if (temp_shell.commands)
			free_commands(temp_shell.commands);
		return NULL;
	}
	
	// Free the temporary tokens but keep the commands
	free_tokens(subshell_tokens);
	
	return temp_shell.commands;
}

int	parse_tokens(t_shell *shell)
{
	t_token		*token;
	t_command	*cmd;
	t_command	*cmd_head;
	t_token     *closing_paren;

	token = shell->tokens;
	if (!token)
		return (0);
	cmd = create_command();
	cmd_head = cmd;
	while (token)
	{
		if (token->type == TOKEN_LPAREN)
		{
			// Find matching closing parenthesis
			closing_paren = find_matching_paren(token);
			if (!closing_paren)
			{
				ft_putendl_fd("Error: Syntax error: unclosed parenthesis", 2);
				free_commands(cmd_head);
				return (1);
			}
			
			// Parse the subshell
			cmd->is_subshell = 1;
			cmd->subshell = parse_subshell(token, closing_paren);
			
			if (!cmd->subshell)
			{
				free_commands(cmd_head);
				return (1);
			}
			
			// Skip to the closing parenthesis
			token = closing_paren;
		}
		else if (token->type == TOKEN_RPAREN)
		{
			// Unexpected closing parenthesis
			ft_putendl_fd("Error: Syntax error near unexpected token `)'", 2);
			free_commands(cmd_head);
			return (1);
		}
		else if (token->type == TOKEN_WORD)
		{
			// If the token value contains wildcards and we're not inside quotes
			if (has_wildcards(token->value))
			{
				char **matches = expand_wildcards(token->value);
				if (matches)
				{
					int i = 0;
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
			}
			else
			{
				// No wildcards, just add the argument as is
				add_arg(cmd, token->value);
			}
		}
		else if (token->type == TOKEN_PIPE)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `|'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_AND)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `&&'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next_op = 1;  // Mark for AND operator
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_OR)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `||'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next_op = 2;  // Mark for OR operator
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT ||
				token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC)
		{
			token = handle_redir(token, cmd);
			if (!token)
			{
				free_commands(cmd_head);
				return (1);
			}
		}
		token = token->next;
	}
	shell->commands = cmd_head;
	return (0);
} 