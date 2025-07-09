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
	char ***new_args, int **new_quoted, int i, int *k)
{
	char	*expanded;

	if (cmd->arg_quoted[i] == 1)
		expanded = ft_strdup(cmd->args[i]);
	else
	{
		expanded = expand_token(shell, cmd->args[i]);
		if (!expanded)
			expanded = ft_strdup(cmd->args[i]);
	}
	(*new_args)[*k] = expanded;
	(*new_quoted)[*k] = cmd->arg_quoted[i];
	(*k)++;
}

/**
 * Handle non-splitting argument expansion
 */
void	handle_non_split_arg(t_shell *shell, t_command *cmd,
	char ***new_args, int **new_quoted, int i, int *k)
{
	char	*expanded;

	if (cmd->arg_quoted && cmd->arg_quoted[i] == 1)
	{
		(*new_args)[*k] = ft_strdup(cmd->args[i]);
		set_quoted_flag(*new_quoted, *k, cmd, i);
		(*k)++;
		return ;
	}
	expanded = expand_token(shell, cmd->args[i]);
	if (!expanded)
	{
		(*new_args)[*k] = ft_strdup(cmd->args[i]);
		set_quoted_flag(*new_quoted, *k, cmd, i);
		(*k)++;
	}
	else
	{
		(*new_args)[*k] = expanded;
		set_quoted_flag(*new_quoted, *k, cmd, i);
		(*k)++;
	}
}

/**
 * Handle case where expansion fails - copy original argument
 */
void	handle_no_expansion(t_command *cmd, char ***new_args,
	int **new_quoted, int i, int *k)
{
	(*new_args)[*k] = ft_strdup(cmd->args[i]);
	set_quoted_flag(*new_quoted, *k, cmd, i);
	(*k)++;
}

/**
 * Handle successful expansion and splitting
 */
void	handle_successful_split(char *expanded, char ***new_args,
	int **new_quoted, int *k)
{
	char	**split_args;
	int		j;

	split_args = ft_split(expanded, ' ');
	if (split_args)
	{
		j = 0;
		while (split_args[j])
		{
			(*new_args)[*k] = split_args[j];
			(*new_quoted)[*k] = 0;
			(*k)++;
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
	char ***new_args, int **new_quoted, int i, int *k)
{
	char	*expanded;

	if (cmd->arg_quoted && cmd->arg_quoted[i] == 1)
	{
		handle_no_expansion(cmd, new_args, new_quoted, i, k);
		return ;
	}
	expanded = expand_token(shell, cmd->args[i]);
	if (!expanded)
		handle_no_expansion(cmd, new_args, new_quoted, i, k);
	else
		handle_successful_split(expanded, new_args, new_quoted, k);
}

/**
 * Process a single argument for expansion
 */
void	process_single_arg(t_shell *shell, t_command *cmd,
	char ***new_args, int **new_quoted, int i, int *k)
{
	int	should_split;

	should_split = should_split_arg(cmd->args[i])
		&& (!cmd->arg_quoted || !cmd->arg_quoted[i]);
	if (cmd->arg_quoted && cmd->arg_quoted[i])
		handle_quoted_arg(shell, cmd, new_args, new_quoted, i, k);
	else if (!should_split)
		handle_non_split_arg(shell, cmd, new_args, new_quoted, i, k);
	else
		handle_split_arg(shell, cmd, new_args, new_quoted, i, k);
}

/**
 * Handle expanded arguments
 */
void	handle_expanded_args(t_shell *shell, t_command *cmd,
	char ***new_args, int **new_quoted)
{
	int		i;
	int		k;

	i = 0;
	k = 0;
	while (cmd->args[i])
	{
		process_single_arg(shell, cmd, new_args, new_quoted, i, &k);
		i++;
	}
	(*new_args)[k] = NULL;
}

/**
 * Check if a single argument needs expansion
 */
int	arg_needs_expansion(t_command *cmd, int i)
{
	if (should_split_arg(cmd->args[i])
		&& (!cmd->arg_quoted || !cmd->arg_quoted[i]))
		return (1);
	if (cmd->arg_quoted && cmd->arg_quoted[i] == 1)
		return (0);
	if (cmd->arg_quoted && cmd->arg_quoted[i]
		&& ft_strchr(cmd->args[i], '$'))
		return (1);
	if (!cmd->arg_quoted && ft_strchr(cmd->args[i], '$'))
		return (1);
	return (0);
}

/**
 * Check if any argument needs expansion
 */
int	check_needs_expansion(t_command *cmd)
{
	int	i;

	if (!cmd || !cmd->args)
		return (0);
	i = 0;
	while (cmd->args[i])
	{
		if (arg_needs_expansion(cmd, i))
			return (1);
		i++;
	}
	return (0);
}

/**
 * Allocate memory for new arguments and quoted flags
 */
int	allocate_new_args_memory(int token_count,
		char ***new_args, int **new_quoted)
{
	*new_args = (char **)malloc(sizeof(char *) * (token_count + 1));
	if (!*new_args)
		return (1);
	*new_quoted = (int *)malloc(sizeof(int) * (token_count + 1));
	if (!*new_quoted)
	{
		free(*new_args);
		return (1);
	}
	return (0);
}

/**
 * Replace command arguments with expanded versions
 */
void	replace_command_args(t_command *cmd, char **new_args, int *new_quoted)
{
	free_cmd_args(cmd->args);
	free(cmd->arg_quoted);
	cmd->args = new_args;
	cmd->arg_quoted = new_quoted;
}

/**
 * Split expanded command if needed
 */
void	split_command_if_needed(t_shell *shell, t_command *cmd)
{
	int		token_count;
	char	**new_args;
	int		*new_quoted;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (!check_needs_expansion(cmd))
		return ;
	token_count = count_split_tokens(shell, cmd);
	if (token_count == 0)
		return ;
	if (allocate_new_args_memory(token_count, &new_args, &new_quoted) != 0)
		return ;
	handle_expanded_args(shell, cmd, &new_args, &new_quoted);
	replace_command_args(cmd, new_args, new_quoted);
}
