#include "../includes/minishell.h"

/**
 * Count number of arguments in the array
 */
int	count_args(char **args)
{
	int	i;

	if (!args)
		return (0);
	i = 0;
	while (args[i])
		i++;
	return (i);
}

/**
 * Handle expansion failure with cleanup
 */
char	**handle_expansion_error(char **new_args, int current_index)
{
	while (--current_index >= 0)
		free(new_args[current_index]);
	free(new_args);
	return (NULL);
}

/**
 * Expand a single argument safely
 */
char	*expand_single_arg(t_shell *shell, char *arg)
{
	char	*expanded;

	expanded = expand_token(shell, arg);
	if (!expanded)
	{
		expanded = ft_strdup("");
		if (!expanded)
			return (NULL);
	}
	return (expanded);
}

// Expand variables in command arguments just before execution
char	**expand_command_args(t_shell *shell, char **args)
{
	int		i;
	int		arg_count;
	char	**new_args;

	arg_count = count_args(args);
	if (arg_count == 0)
		return (NULL);
	new_args = (char **)malloc((arg_count + 1) * sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	while (args[i])
	{
		new_args[i] = expand_single_arg(shell, args[i]);
		if (!new_args[i])
			return (handle_expansion_error(new_args, i));
		i++;
	}
	new_args[i] = NULL;
	return (new_args);
}

// Free the expanded arguments array
void	free_expanded_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}
