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

int	**allocate_pipes(int cmd_count, pid_t **pids)
{
	int	**pipes;
	int	i;

	*pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		print_error("malloc", NULL, strerror(errno));
		return (NULL);
	}
	pipes = (int **)malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		free(*pids);
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

int	setup_pipes(int **pipes, int cmd_count, pid_t *pids)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipe(pipes[i]) == -1)
		{
			while (i >= 0)
			{
				free(pipes[i]);
				i--;
			}
			free(pipes);
			free(pids);
			print_error("pipe", NULL, strerror(errno));
			return (1);
		}
		i++;
	}
	return (0);
}

void	setup_child_pipes(int **pipes, int i, int cmd_count)
{
	int	j;

	if (i > 0)
	{
		if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
		{
			perror("dup2 stdin");
			exit(1);
		}
	}
	if (i < cmd_count - 1)
	{
		if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
		{
			perror("dup2 stdout");
			exit(1);
		}
	}
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
