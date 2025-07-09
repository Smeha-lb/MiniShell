/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_logic6.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:59:50 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 15:59:51 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
