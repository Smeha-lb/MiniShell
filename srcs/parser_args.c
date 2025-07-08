#include "../includes/minishell.h"

// Helper function for add_arg when cmd->args is NULL
void	init_cmd_args(t_command *cmd, char *arg, int quoted)
{
	cmd->args = (char **)malloc(2 * sizeof(char *));
	if (!cmd->args)
	{
		free(arg);
		return;
	}
	
	cmd->arg_quoted = (int *)malloc(1 * sizeof(int));
	if (!cmd->arg_quoted)
	{
		free(cmd->args);
		free(arg);
		cmd->args = NULL;
		return;
	}
	
	cmd->args[0] = arg;
	cmd->args[1] = NULL;
	cmd->arg_quoted[0] = quoted;
}

/**
 * Allocate memory for expanded argument arrays
 */
int	allocate_expanded_arg_arrays(int count, char ***new_args, int **new_quoted)
{
	*new_args = (char **)malloc((count + 2) * sizeof(char *));
	if (!*new_args)
		return (1);
	*new_quoted = (int *)malloc((count + 1) * sizeof(int));
	if (!*new_quoted)
	{
		free(*new_args);
		return (1);
	}
	return (0);
}

/**
 * Copy existing arguments to new arrays and add new argument
 */
void	copy_and_add_arg(t_command *cmd, char *arg, int quoted,
	char **new_args, int *new_quoted)
{
	int	i;

	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		new_quoted[i] = cmd->arg_quoted[i];
		i++;
	}
	new_args[i] = arg;
	new_args[i + 1] = NULL;
	new_quoted[i] = quoted;
}

// Helper function for add_arg when cmd->args already exists
void	append_cmd_arg(t_command *cmd, char *arg, int quoted)
{
	int		i;
	char	**new_args;
	int		*new_quoted;

	i = 0;
	while (cmd->args[i])
		i++;
	if (allocate_expanded_arg_arrays(i, &new_args, &new_quoted) != 0)
	{
		free(arg);
		return;
	}
	copy_and_add_arg(cmd, arg, quoted, new_args, new_quoted);
	free(cmd->args);
	free(cmd->arg_quoted);
	cmd->args = new_args;
	cmd->arg_quoted = new_quoted;
}

// Store the original argument without expansion
void	add_arg(t_command *cmd, char *arg)
{
	add_arg_with_quoted(cmd, arg, 0);
}

void	add_arg_with_quoted(t_command *cmd, char *arg, int quoted)
{
	char	*arg_copy;

	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return ;
	if (!cmd->args)
		init_cmd_args(cmd, arg_copy, quoted);
	else
		append_cmd_arg(cmd, arg_copy, quoted);
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
