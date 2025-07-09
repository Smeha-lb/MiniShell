#include "../includes/minishell.h"
#include <signal.h>

static int	handle_eof_or_signal(void)
{
	if (g_signal_code == 130)
		return (130);
	return (0);
}

static int	check_delimiter(char *line, char *delimiter)
{
	if (ft_strcmp(line, delimiter) == 0)
	{
		free(line);
		return (1);
	}
	return (0);
}

static void	write_line_to_fd(t_shell *shell, char *line, int fd, int quoted)
{
	char	*expanded_line;

	if (quoted)
	{
		ft_putendl_fd(line, fd);
		free(line);
		return ;
	}
	expanded_line = expand_variables(shell, line, 0);
	if (expanded_line)
	{
		ft_putendl_fd(expanded_line, fd);
		free(expanded_line);
	}
	else
		ft_putendl_fd(line, fd);
	free(line);
}

static int	process_heredoc_lines(t_shell *shell,
		char *delimiter, int fd, int quoted)
{
	char	*line;

	while (1)
	{
		line = readline(HEREDOC_PROMPT);
		if (!line)
			return (handle_eof_or_signal());
		if (check_delimiter(line, delimiter))
			break ;
		write_line_to_fd(shell, line, fd, quoted);
	}
	return (0);
}

int	handle_heredoc(t_shell *shell, t_redir *redir, char **temp_file)
{
	int	fd;
	int	result;

	*temp_file = create_heredoc_tempfile();
	if (!*temp_file)
	{
		print_error("malloc", NULL, strerror(errno));
		return (1);
	}
	fd = open(*temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		print_error(*temp_file, NULL, strerror(errno));
		free(*temp_file);
		*temp_file = NULL;
		return (1);
	}
	setup_heredoc_signals();
	result = process_heredoc_lines(shell, redir->file, fd, redir->quoted);
	setup_signals();
	close(fd);
	return (result);
}
