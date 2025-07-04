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

static char	*create_heredoc_filename(int counter, pid_t pid)
{
	char	*num_str;
	char	*pid_str;
	char	*temp1;
	char	*temp2;
	char	*filename;

	num_str = ft_itoa(counter);
	pid_str = ft_itoa(pid);
	if (!num_str || !pid_str)
	{
		free(num_str);
		free(pid_str);
		return (NULL);
	}
	temp1 = ft_strjoin("/tmp/minishell_heredoc_", pid_str);
	if (!temp1)
	{
		free(num_str);
		free(pid_str);
		return (NULL);
	}
	temp2 = ft_strjoin(temp1, "_");
	if (!temp2)
	{
		free(num_str);
		free(pid_str);
		free(temp1);
		return (NULL);
	}
	filename = ft_strjoin(temp2, num_str);
	free(num_str);
	free(pid_str);
	free(temp1);
	free(temp2);
	return (filename);
}

char	*create_heredoc_tempfile(void)
{
	static int	counter = 0;

	return (create_heredoc_filename(counter++, getpid()));
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
