#include "../includes/minishell.h"

/**
 * Handle case where expansion fails - copy original argument
 */
void	handle_no_expansion(t_command *cmd, t_arg_expansion_data *data, int i)
{
	(*data->new_args)[*data->k] = ft_strdup(cmd->args[i]);
	set_quoted_flag(*data->new_quoted, *data->k, cmd, i);
	(*data->k)++;
}

/**
 * Handle successful expansion and splitting
 */
void	handle_successful_split(char *expanded, t_arg_expansion_data *data)
{
	char	**split_args;
	int		j;

	split_args = ft_split(expanded, ' ');
	if (split_args)
	{
		j = 0;
		while (split_args[j])
		{
			(*data->new_args)[*data->k] = split_args[j];
			(*data->new_quoted)[*data->k] = 0;
			(*data->k)++;
			j++;
		}
		free(split_args);
	}
	free(expanded);
}

/**
 * Handle splitting argument expansion
 */
void	handle_split_arg(t_shell *shell, t_command *cmd,
	t_arg_expansion_data *data, int i)
{
	char	*expanded;

	if (cmd->arg_quoted && cmd->arg_quoted[i] == 1)
	{
		handle_no_expansion(cmd, data, i);
		return ;
	}
	expanded = expand_token(shell, cmd->args[i]);
	if (!expanded)
		handle_no_expansion(cmd, data, i);
	else
		handle_successful_split(expanded, data);
}

/**
 * Process a single argument for expansion
 */
void	process_single_arg(t_shell *shell, t_command *cmd,
	t_arg_expansion_data *data, int i)
{
	int	should_split;

	should_split = should_split_arg(cmd->args[i])
		&& (!cmd->arg_quoted || !cmd->arg_quoted[i]);
	if (cmd->arg_quoted && cmd->arg_quoted[i])
		handle_quoted_arg(shell, cmd, data, i);
	else if (!should_split)
		handle_non_split_arg(shell, cmd, data, i);
	else
		handle_split_arg(shell, cmd, data, i);
}

/**
 * Handle expanded arguments
 */
void	handle_expanded_args(t_shell *shell, t_command *cmd,
	char ***new_args, int **new_quoted)
{
	int						i;
	int						k;
	t_arg_expansion_data	data;

	i = 0;
	k = 0;
	data.new_args = new_args;
	data.new_quoted = new_quoted;
	data.k = &k;
	while (cmd->args[i])
	{
		process_single_arg(shell, cmd, &data, i);
		i++;
	}
	(*new_args)[k] = NULL;
}
