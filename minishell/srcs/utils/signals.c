/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status;

/**
 * Signal handler for interactive mode
 */
static void	handle_interactive(int sig)
{
	if (sig == SIGINT)
	{
		ft_putstr_fd("\n", 1);
		g_exit_status = 130;
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * Signal handler for child processes
 */
static void	handle_child(int sig)
{
	if (sig == SIGINT)
	{
		ft_putstr_fd("\n", 1);
		g_exit_status = 130;
	}
	else if (sig == SIGQUIT)
	{
		ft_putstr_fd("Quit (core dumped)\n", 1);
		g_exit_status = 131;
	}
}

/**
 * Signal handler for heredoc
 */
static void	handle_heredoc(int sig)
{
	if (sig == SIGINT)
	{
		ft_putstr_fd("\n", 1);
		g_exit_status = 130;
		exit(130);
	}
}

/**
 * Setup signals based on the current mode:
 * 0: Interactive mode
 * 1: Child process mode
 * 2: Heredoc mode
 */
void	setup_signals(int mode)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	if (mode == 0)
	{
		term.c_lflag &= ~ECHOCTL;
		signal(SIGINT, handle_interactive);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == 1)
	{
		term.c_lflag |= ECHOCTL;
		signal(SIGINT, handle_child);
		signal(SIGQUIT, handle_child);
	}
	else if (mode == 2)
	{
		term.c_lflag &= ~ECHOCTL;
		signal(SIGINT, handle_heredoc);
		signal(SIGQUIT, SIG_IGN);
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
} 