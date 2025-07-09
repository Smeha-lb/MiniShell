/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_pipeline3.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:21 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:22 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	check_nested_minishell(t_command *start_cmd)
{
	t_command	*cmd;
	int			is_nested;

	is_nested = 0;
	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		if (cmd->args && cmd->args[0]
			&& (ft_strcmp(cmd->args[0], "./minishell") == 0
				|| ft_strcmp(cmd->args[0], "minishell") == 0))
			is_nested = 1;
		cmd = cmd->next;
	}
	return (is_nested);
}

int	count_pipeline_cmds(t_command *start_cmd)
{
	t_command	*cmd;
	int			count;

	count = 1;
	cmd = start_cmd;
	while (cmd->next && cmd->next_op == 0)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}
