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

int	update_heredoc_redirs(t_command *cmd, char **heredoc_tempfiles)
{
	t_redir	*redir;
	int		i;

	i = 0;
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			free(redir->file);
			redir->file = ft_strdup(heredoc_tempfiles[i]);
			if (!redir->file)
				return (1);
			redir->type = TOKEN_REDIR_IN;
			i++;
		}
		redir = redir->next;
	}
	return (0);
}

/**
 * Handle heredoc processing with error cleanup
 */
int	process_heredocs_with_cleanup(t_shell *shell, t_command *cmd,
	char **heredoc_tempfiles, int heredoc_count)
{
	int	result;

	result = process_heredocs(shell, cmd, heredoc_tempfiles);
	if (result != 0)
	{
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
		return (result);
	}
	if (update_heredoc_redirs(cmd, heredoc_tempfiles) != 0)
	{
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
		return (1);
	}
	return (0);
}

/**
 * Process heredocs for a single command
 */
int	process_cmd_heredocs(t_shell *shell, t_command *cmd)
{
	int		heredoc_count;
	char	**heredoc_tempfiles;
	int		result;
	int		i;

	i = 0;
	heredoc_count = 0;
	heredoc_count = count_heredocs(cmd);
	if (heredoc_count <= 0)
		return (0);
	heredoc_tempfiles = init_heredoc_tempfiles(heredoc_count);
	if (!heredoc_tempfiles)
		return (1);
	result = process_heredocs_with_cleanup(shell, cmd,
			heredoc_tempfiles, heredoc_count);
	if (result != 0)
		return (result);
	while (i < heredoc_count)
	{
		free(heredoc_tempfiles[i]);
		i++;
	}
	free(heredoc_tempfiles);
	return (0);
}

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
