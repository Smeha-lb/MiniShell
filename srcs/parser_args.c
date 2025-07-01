#include "../includes/minishell.h"

// Helper function for add_arg when cmd->args is NULL
void	init_cmd_args(t_command *cmd, char *arg)
{
	cmd->args = (char **)malloc(2 * sizeof(char *));
	if (!cmd->args)
	{
		free(arg);
		return ;
	}
	cmd->args[0] = arg;
	cmd->args[1] = NULL;
}

// Helper function for add_arg when cmd->args already exists
void	append_cmd_arg(t_command *cmd, char *arg)
{
	int		i;
	char	**new_args;

	i = 0;
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc((i + 2) * sizeof(char *));
	if (!new_args)
	{
		free(arg);
		return ;
	}
	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = arg;
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

// Store the original argument without expansion
void	add_arg(t_command *cmd, char *arg)
{
	char	*arg_copy;

	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return;
		
	if (!cmd->args)
		init_cmd_args(cmd, arg_copy);
	else
		append_cmd_arg(cmd, arg_copy);
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
	free_array(matches);
}

// Helper function to handle word tokens with wildcards
// If wildcard expansion fails, use the original token value
bool	handle_wildcards_token(t_token *token, t_command *cmd)
{
	// Store the original token value with wildcards
	add_arg(cmd, token->value);
	return (true);
}
