#include "../includes/minishell.h"

static int	get_output_flags(int append)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	return (flags);
}

int	redirect_input(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		print_error(file, NULL, strerror(errno));
		return (1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	redirect_output(char *file, int append)
{
	int	fd;
	int	flags;

	flags = get_output_flags(append);
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		print_error(file, NULL, strerror(errno));
		return (1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(fd);
		return (1);
	}
	close(fd);
	return (0);
}

int	apply_redirection(t_redir *redir,
		char **heredoc_tempfiles, int *heredoc_index)
{
	if (redir->type == TOKEN_REDIR_IN)
		return (redirect_input(redir->file));
	else if (redir->type == TOKEN_REDIR_OUT)
		return (redirect_output(redir->file, 0));
	else if (redir->type == TOKEN_REDIR_APPEND)
		return (redirect_output(redir->file, 1));
	else if (redir->type == TOKEN_HEREDOC)
		return (apply_heredoc_redirection(
				heredoc_tempfiles[(*heredoc_index)++]));
	return (0);
}

int	apply_all_redirections(t_command *cmd, char **heredoc_tempfiles)
{
	t_redir	*redir;
	int		heredoc_index;
	int		result;

	heredoc_index = 0;
	redir = cmd->redirs;
	while (redir)
	{
		result = apply_redirection(redir, heredoc_tempfiles, &heredoc_index);
		if (result != 0)
			return (result);
		redir = redir->next;
	}
	return (0);
}
