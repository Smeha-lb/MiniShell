#include "../includes/minishell.h"

void	free_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipes[i][0] >= 0)
			close(pipes[i][0]);
		if (pipes[i][1] >= 0)
			close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

void	init_pipeline(t_pipeline *pipeline
		, t_shell *shell, t_command *start_cmd)
{
	pipeline->shell = shell;
	pipeline->start_cmd = start_cmd;
	pipeline->is_nested = check_nested_minishell(start_cmd);
	pipeline->cmd_count = count_pipeline_cmds(start_cmd);
}

void	fork_pipeline_processes(t_pipeline *pipeline, t_command *start_cmd)
{
	t_command			*cmd;
	t_pipeline_child	pc;
	int					i;

	cmd = start_cmd;
	i = 0;
	while (i < pipeline->cmd_count)
	{
		pipeline->pids[i] = fork();
		if (pipeline->pids[i] == -1)
			print_error("fork", NULL, strerror(errno));
		else if (pipeline->pids[i] == 0)
		{
			pc.shell = pipeline->shell;
			pc.cmd = cmd;
			pc.pipes = pipeline->pipes;
			pc.i = i;
			pc.cmd_count = pipeline->cmd_count;
			handle_pipeline_child(&pc);
		}
		cmd = cmd->next;
		i++;
	}
}

int	execute_pipeline(t_shell *shell, t_command *start_cmd)
{
	t_pipeline	pipeline;
	int			exit_status;

	exit_status = 0;
	init_pipeline(&pipeline, shell, start_cmd);
	pipeline.pipes = allocate_pipes(pipeline.cmd_count, &pipeline.pids);
	if (!pipeline.pipes)
		return (1);
	if (setup_pipes(pipeline.pipes, pipeline.cmd_count, pipeline.pids))
	{
		free(pipeline.pids);
		return (1);
	}
	fork_pipeline_processes(&pipeline, start_cmd);
	free_pipes(pipeline.pipes, pipeline.cmd_count);
	exit_status = wait_for_pipeline(pipeline.pids,
			pipeline.cmd_count, pipeline.is_nested);
	free(pipeline.pids);
	return (exit_status);
}

int	wait_for_all(pid_t last_pid)
{
	int	status;
	int	exit_status;

	(void)last_pid;
	exit_status = 0;
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_status = 128 + WTERMSIG(status);
	}
	return (exit_status);
}
