#include "../includes/minishell.h"

/**
 * Count tokens from a single expanded and split argument
 */
int	count_tokens_from_arg(t_shell *shell, char *arg)
{
	char	*expanded;
	char	**split_args;
	int		count;
	int		j;

	expanded = expand_token(shell, arg);
	if (!expanded)
		return (1);
	split_args = ft_split(expanded, ' ');
	count = 0;
	if (split_args)
	{
		j = 0;
		while (split_args[j])
		{
			count++;
			j++;
		}
		free_array(split_args);
	}
	free(expanded);
	return (count);
}

/**
 * Count the number of tokens after expansion and splitting
 */
int	count_split_tokens(t_shell *shell, t_command *cmd)
{
	int		i;
	int		total_tokens;
	int		should_split;

	if (!cmd || !cmd->args)
		return (0);
	total_tokens = 0;
	i = 0;
	while (cmd->args[i])
	{
		should_split = should_split_arg(cmd->args[i])
			&& (!cmd->arg_quoted || !cmd->arg_quoted[i]);
		if (!should_split)
			total_tokens++;
		else
			total_tokens += count_tokens_from_arg(shell, cmd->args[i]);
		i++;
	}
	return (total_tokens);
}

/**
 * Set quoted flag for a new argument
 */
void	set_quoted_flag(int *new_quoted, int k, t_command *cmd, int i)
{
	if (cmd->arg_quoted)
		new_quoted[k] = cmd->arg_quoted[i];
	else
		new_quoted[k] = 0;
}

/**
 * Handle quoted argument expansion
 */
void	handle_quoted_arg(t_shell *shell, t_command *cmd,
	t_arg_expansion_data *data, int i)
{
	char	*expanded;

	if (cmd->arg_quoted[i] == 1 || cmd->arg_quoted[i] == 3)
		expanded = ft_strdup(cmd->args[i]);
	else
	{
		expanded = expand_token(shell, cmd->args[i]);
		if (!expanded)
			expanded = ft_strdup(cmd->args[i]);
	}
	(*data->new_args)[*data->k] = expanded;
	(*data->new_quoted)[*data->k] = cmd->arg_quoted[i];
	(*data->k)++;
}

/**
 * Handle non-splitting argument expansion
 */
void	handle_non_split_arg(t_shell *shell, t_command *cmd,
	t_arg_expansion_data *data, int i)
{
	char	*expanded;

	if (cmd->arg_quoted && (cmd->arg_quoted[i] == 1 || cmd->arg_quoted[i] == 3))
	{
		(*data->new_args)[*data->k] = ft_strdup(cmd->args[i]);
		set_quoted_flag(*data->new_quoted, *data->k, cmd, i);
		(*data->k)++;
		return ;
	}
	expanded = expand_token(shell, cmd->args[i]);
	if (!expanded)
	{
		(*data->new_args)[*data->k] = ft_strdup(cmd->args[i]);
		set_quoted_flag(*data->new_quoted, *data->k, cmd, i);
		(*data->k)++;
	}
	else
	{
		(*data->new_args)[*data->k] = expanded;
		set_quoted_flag(*data->new_quoted, *data->k, cmd, i);
		(*data->k)++;
	}
}
