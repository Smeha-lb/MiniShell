/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status;

/**
 * Create pipes for a pipeline
 */
static int	create_pipes(int **pipes, int cmd_count)
{
	int	i;

	*pipes = (int *)malloc(sizeof(int) * 2 * (cmd_count - 1));
	if (!*pipes)
		return (0);
	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe((*pipes) + i * 2) < 0)
		{
			free(*pipes);
			return (0);
		}
		i++;
	}
	return (1);
}

/**
 * Close all pipes
 */
static void	close_pipes(int *pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < 2 * (cmd_count - 1))
	{
		close(pipes[i]);
		i++;
	}
}

/**
 * Setup pipes for a command in a pipeline
 */
static void	setup_pipes(int *pipes, int cmd_index, int cmd_count)
{
	if (cmd_index > 0)
	{
		dup2(pipes[(cmd_index - 1) * 2], STDIN_FILENO);
	}
	if (cmd_index < cmd_count - 1)
	{
		dup2(pipes[cmd_index * 2 + 1], STDOUT_FILENO);
	}
	close_pipes(pipes, cmd_count);
}

/**
 * Count the number of commands in a pipeline
 */
static int	count_commands(t_command *commands)
{
	int			count;
	t_command	*cmd;

	count = 0;
	cmd = commands;
	while (cmd)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

/**
 * Execute a pipeline of commands
 */
int	execute_pipeline(t_minishell *shell, t_command *commands)
{
	int			cmd_count;
	int			*pipes;
	int			i;
	t_command	*cmd;
	pid_t		*pids;
	int			status;
	int			exit_status;

	cmd_count = count_commands(commands);
	if (cmd_count == 1)
		return (execute_command(shell, commands));
	pipes = NULL;
	if (!create_pipes(&pipes, cmd_count))
	{
		print_error("pipe", NULL);
		g_exit_status = 1;
		return (1);
	}
	pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		close_pipes(pipes, cmd_count);
		free(pipes);
		g_exit_status = 1;
		return (1);
	}
	setup_signals(1);
	i = 0;
	cmd = commands;
	while (cmd)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			print_error("fork", NULL);
			close_pipes(pipes, cmd_count);
			free(pipes);
			free(pids);
			g_exit_status = 1;
			return (1);
		}
		else if (pids[i] == 0)
		{
			setup_pipes(pipes, i, cmd_count);
			setup_redirections(cmd->redirections, shell);
			exit(execute_command(shell, cmd));
		}
		cmd = cmd->next;
		i++;
	}
	close_pipes(pipes, cmd_count);
	free(pipes);
	i = 0;
	exit_status = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	free(pids);
	setup_signals(0);
	g_exit_status = exit_status;
	return (exit_status);
} 