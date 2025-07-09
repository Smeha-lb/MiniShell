/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_pipes3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:33 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:34 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Cleanup on pipe creation failure
 */
void	cleanup_pipes_on_error(int **pipes, int current_index, pid_t *pids)
{
	while (current_index >= 0)
	{
		free(pipes[current_index]);
		current_index--;
	}
	free(pipes);
	free(pids);
}

/**
 * Create all pipes in the array
 */
int	create_all_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes[i]) == -1)
		{
			print_error("pipe", NULL, strerror(errno));
			return (i);
		}
		i++;
	}
	return (-1);
}
