#include "../includes/minishell.h"

// Helper function for add_arg when cmd->args is NULL
void	init_cmd_args(t_command *cmd, char *expanded_arg)
{
	cmd->args = (char **)malloc(2 * sizeof(char *));
	if (!cmd->args)
	{
		free(expanded_arg);
		return ;
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
		return ;
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

	expanded_arg = ft_strdup(arg);
	if (!cmd->args)
		init_cmd_args(cmd, expanded_arg);
	else
		append_cmd_arg(cmd, expanded_arg);
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
