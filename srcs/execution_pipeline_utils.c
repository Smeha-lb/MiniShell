#include "../includes/minishell.h"

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
