#include "../includes/minishell.h"

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

	flags = O_WRONLY | O_CREAT;
	if (append)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
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

int	handle_heredoc(t_shell *shell, t_redir *redir)
{
	int		pipefd[2];
	char	*line;
	char	*delimiter;

	(void)shell; // Unused parameter
	delimiter = redir->file;
	if (pipe(pipefd) == -1)
	{
		print_error("pipe", NULL, strerror(errno));
		return (1);
	}
	
	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
		{
			ft_putendl_fd("warning: here-document delimited by end-of-file", 2);
			break;
		}
		if (ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		ft_putendl_fd(line, pipefd[1]);
		free(line);
	}
	
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		print_error("dup2", NULL, strerror(errno));
		close(pipefd[0]);
		return (1);
	}
	close(pipefd[0]);
	return (0);
}

int	setup_redirections(t_command *cmd)
{
	t_redir	*redir;

	if (!cmd)
		return (0);
	redir = cmd->redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIR_IN)
		{
			if (redirect_input(redir->file) != 0)
				return (1);
		}
		else if (redir->type == TOKEN_REDIR_OUT)
		{
			if (redirect_output(redir->file, 0) != 0)
				return (1);
		}
		else if (redir->type == TOKEN_REDIR_APPEND)
		{
			if (redirect_output(redir->file, 1) != 0)
				return (1);
		}
		else if (redir->type == TOKEN_HEREDOC)
		{
			if (handle_heredoc(NULL, redir) != 0)
				return (1);
		}
		redir = redir->next;
	}
	return (0);
} 