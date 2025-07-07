#include "../includes/minishell.h"

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

int	process_all_pipeline_heredocs(t_shell *shell, t_command *start_cmd)
{
	t_command	*cmd;
	int			heredoc_count;
	char		**heredoc_tempfiles;
	int			result;

	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		heredoc_count = count_heredocs(cmd);
		if (heredoc_count > 0)
		{
			heredoc_tempfiles = init_heredoc_tempfiles(heredoc_count);
			if (!heredoc_tempfiles)
				return (1);
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
			cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
		}
		cmd = cmd->next;
	}
	return (0);
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