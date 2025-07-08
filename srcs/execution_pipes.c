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
