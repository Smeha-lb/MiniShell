#include "../includes/minishell.h"
#include <signal.h>

static void	heredoc_sig_handler(int signum)
{
	if (signum == SIGINT)
	{
		g_signal_code = 130;
		ft_putchar_fd('\n', STDOUT_FILENO);
		close(STDIN_FILENO);
	}
}

void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_sig_handler);
	signal(SIGQUIT, SIG_IGN);
	g_signal_code = 0;
}

static char	*create_heredoc_filename(int counter)
{
	char	*num_str;
	char	*filename;

	num_str = ft_itoa(counter);
	if (!num_str)
		return (NULL);
	filename = ft_strjoin("/tmp/minishell_heredoc_", num_str);
	free(num_str);
	return (filename);
}

char	*create_heredoc_tempfile(void)
{
	static int	counter = 0;

	return (create_heredoc_filename(counter++));
}

int	apply_heredoc_redirection(char *temp_file)
{
	int	fd;

	fd = open(temp_file, O_RDONLY);
	if (fd == -1)
	{
		print_error(temp_file, NULL, strerror(errno));
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
