/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_heredoc.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:13:04 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:13:04 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

/**
 * Build the heredoc filename from components
 */
char	*build_heredoc_path(char *num_str, char *pid_str)
{
	char	*temp1;
	char	*temp2;
	char	*filename;

	temp1 = ft_strjoin("/tmp/minishell_heredoc_", pid_str);
	if (!temp1)
		return (NULL);
	temp2 = ft_strjoin(temp1, "_");
	if (!temp2)
	{
		free(temp1);
		return (NULL);
	}
	filename = ft_strjoin(temp2, num_str);
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
