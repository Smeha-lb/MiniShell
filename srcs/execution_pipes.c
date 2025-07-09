#include "../includes/minishell.h"

int	create_pipes(t_command *commands)
{
	int			pipefd[2];
	t_command	*current;
	t_command	*next;

	current = commands;
	while (current && current->next && current->next_op == 0)
	{
		next = current->next;
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", NULL, strerror(errno));
			return (1);
		}
		current->pipe_out = pipefd[1];
		next->pipe_in = pipefd[0];
		current = next;
	}
	return (0);
}

void	close_pipes(t_command *commands)
{
	t_command	*current;

	current = commands;
	while (current)
	{
		if (current->pipe_in != -1)
			close(current->pipe_in);
		if (current->pipe_out != -1)
			close(current->pipe_out);
		current = current->next;
	}
}

/**
 * Allocate process ID array
 */
pid_t	*allocate_pids(int cmd_count)
{
	pid_t	*pids;

	pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
		print_error("malloc", NULL, strerror(errno));
	return (pids);
}

/**
 * Allocate individual pipe arrays
 */
int	**allocate_pipe_arrays(int cmd_count, pid_t *pids)
{
	int	**pipes;
	int	i;

	pipes = (int **)malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		free(pids);
		print_error("malloc", NULL, strerror(errno));
		return (NULL);
	}
	i = 0;
	while (i < cmd_count - 1)
	{
		pipes[i] = (int *)malloc(sizeof(int) * 2);
		if (!pipes[i])
			return (NULL);
		i++;
	}
	return (pipes);
}

int	**allocate_pipes(int cmd_count, pid_t **pids)
{
	*pids = allocate_pids(cmd_count);
	if (!*pids)
		return (NULL);
	return (allocate_pipe_arrays(cmd_count, *pids));
}
