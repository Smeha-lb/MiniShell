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
		// Ignore SIGTSTP (Ctrl+Z) in the shell
		g_signal_code = 0;
		rl_redisplay();
	}
}

void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_signals;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	// Set up SIGINT (CTRL-C) to display a new prompt on a new line
	sigaction(SIGINT, &sa, NULL);
	
	// Make SIGQUIT (CTRL-\) behave the same as SIGINT
	sigaction(SIGQUIT, &sa, NULL);
	
	// Ignore SIGTSTP (CTRL-Z) to prevent suspending the shell
	sigaction(SIGTSTP, &sa, NULL);
} 