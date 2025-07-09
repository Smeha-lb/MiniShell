#include "../includes/minishell.h"

int	process_all_pipeline_heredocs(t_shell *shell, t_command *start_cmd)
{
	t_command	*cmd;
	int			result;

	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		result = process_cmd_heredocs(shell, cmd);
		if (result != 0)
			return (result);
		cmd = cmd->next;
	}
	return (0);
}

/**
 * Setup pipeline infrastructure (pipes and process arrays)
 */
int	setup_pipeline_infrastructure(t_pipeline *pipeline)
{
	pipeline->pipes = allocate_pipes(pipeline->cmd_count, &pipeline->pids);
	if (!pipeline->pipes)
		return (1);
	if (setup_pipes(pipeline->pipes, pipeline->cmd_count, pipeline->pids))
	{
		free(pipeline->pids);
		return (1);
	}
	return (0);
}

/**
 * Execute and wait for pipeline completion
 */
int	run_pipeline(t_pipeline *pipeline, t_command *start_cmd)
{
	int	exit_status;

	fork_pipeline_processes(pipeline, start_cmd);
	free_pipes(pipeline->pipes, pipeline->cmd_count);
	exit_status = wait_for_pipeline(pipeline->pids,
			pipeline->cmd_count, pipeline->is_nested);
	free(pipeline->pids);
	return (exit_status);
}

int	execute_pipeline(t_shell *shell, t_command *start_cmd)
{
	t_pipeline	pipeline;

	if (process_all_pipeline_heredocs(shell, start_cmd) != 0)
		return (1);
	init_pipeline(&pipeline, shell, start_cmd);
	if (setup_pipeline_infrastructure(&pipeline) != 0)
		return (1);
	return (run_pipeline(&pipeline, start_cmd));
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
