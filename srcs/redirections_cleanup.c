#include "../includes/minishell.h"

static void	cleanup_single_tempfile(char *temp_file)
{
	if (temp_file)
	{
		unlink(temp_file);
		free(temp_file);
	}
}

void	cleanup_heredoc_tempfiles(char **temp_files, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		cleanup_single_tempfile(temp_files[i]);
		i++;
	}
	free(temp_files);
}

int	count_heredocs(t_command *cmd)
{
	t_redir	*redir;
	int		count;

	count = 0;
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
			count++;
		redir = redir->next;
	}
	return (count);
}

char	**init_heredoc_tempfiles(int count)
{
	char	**tempfiles;
	int		i;

	tempfiles = (char **)malloc(sizeof(char *) * count);
	if (!tempfiles)
	{
		print_error("malloc", NULL, strerror(errno));
		return (NULL);
	}
	i = 0;
	while (i < count)
	{
		tempfiles[i] = NULL;
		i++;
	}
	return (tempfiles);
}

int	process_heredocs(t_shell *shell, t_command *cmd, char **tempfiles)
{
	t_redir	*redir;
	int		i;
	int		result;

	i = 0;
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_HEREDOC)
		{
			result = handle_heredoc(shell, redir, &tempfiles[i]);
			if (result != 0)
				return (result);
			i++;
		}
		redir = redir->next;
	}
	return (0);
}
