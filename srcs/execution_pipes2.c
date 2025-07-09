/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_pipes2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:30 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:31 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	setup_pipes(int **pipes, int cmd_count, pid_t *pids)
{
	int	failed_index;

	failed_index = create_all_pipes(pipes, cmd_count);
	if (failed_index >= 0)
	{
		cleanup_pipes_on_error(pipes, failed_index, pids);
		return (1);
	}
	return (0);
}

/**
 * Setup input pipe for child process
 */
void	setup_child_input(int **pipes, int i)
{
	if (i > 0)
	{
		if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
		{
			perror("dup2 stdin");
			exit(1);
		}
	}
}

/**
 * Setup output pipe for child process
 */
void	setup_child_output(int **pipes, int i, int cmd_count)
{
	if (i < cmd_count - 1)
	{
		if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
		{
			perror("dup2 stdout");
			exit(1);
		}
	}
}

/**
 * Close all pipe file descriptors
 */
void	close_all_pipes(int **pipes, int cmd_count)
{
	int	j;

	j = 0;
	while (j < cmd_count - 1)
	{
		if (pipes[j][0] >= 0)
			close(pipes[j][0]);
		if (pipes[j][1] >= 0)
			close(pipes[j][1]);
		j++;
	}
}

void	setup_child_pipes(int **pipes, int i, int cmd_count)
{
	setup_child_input(pipes, i);
	setup_child_output(pipes, i, cmd_count);
	close_all_pipes(pipes, cmd_count);
}
