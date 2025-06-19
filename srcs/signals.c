#include "../includes/minishell.h"

void	handle_signals(int signum)
{
	if (signum == SIGINT || signum == SIGQUIT)
	{
		g_signal_code = 1;
		ft_putchar_fd('\n', STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (signum == SIGTSTP)
	{
		g_signal_code = 0;
		rl_redisplay();
	}
}

// Set up signal handling for the shell
void	setup_signals(void)
{
	signal(SIGINT, handle_signals);
	signal(SIGQUIT, handle_signals);
	signal(SIGTSTP, handle_signals);
}

void	ignore_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
}
