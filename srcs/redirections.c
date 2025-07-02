#include "../includes/minishell.h"
#include <signal.h>

int	get_output_flags(int append)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	return (flags);
}

static int	setup_heredoc_tempfiles(t_shell *shell, t_command *cmd,
			char ***heredoc_tempfiles, int heredoc_count)
{
	int	result;

	*heredoc_tempfiles = init_heredoc_tempfiles(heredoc_count);
	if (!*heredoc_tempfiles)
		return (1);
	result = process_heredocs(shell, cmd, *heredoc_tempfiles);
	if (result != 0)
	{
		cleanup_heredoc_tempfiles(*heredoc_tempfiles, heredoc_count);
		return (result);
	}
	return (0);
}

static int	handle_redirection_result(int result, char **heredoc_tempfiles,
			int heredoc_count)
{
	if (result != 0 && heredoc_tempfiles)
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
	return (result);
}

int	setup_redirections(t_shell *shell, t_command *cmd)
{
	int		heredoc_count;
	char	**heredoc_tempfiles;
	int		result;

	heredoc_count = 0;
	heredoc_tempfiles = NULL;
	if (!cmd)
		return (0);
	heredoc_count = count_heredocs(cmd);
	if (heredoc_count > 0)
	{
		result = setup_heredoc_tempfiles(shell, cmd,
				&heredoc_tempfiles, heredoc_count);
		if (result != 0)
			return (result);
	}
	result = apply_all_redirections(shell, cmd, heredoc_tempfiles);
	if (result != 0)
		return (handle_redirection_result(result,
				heredoc_tempfiles, heredoc_count));
	if (heredoc_tempfiles)
		cleanup_heredoc_tempfiles(heredoc_tempfiles, heredoc_count);
	return (0);
}
