#include "../includes/minishell.h"

// Global variable to store original signal handlers
struct sigaction g_original_int;
struct sigaction g_original_quit;
struct sigaction g_original_tstp;

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

// Set up signal handling for the shell
void	setup_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_signals;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	// Store original signal handlers
	sigaction(SIGINT, NULL, &g_original_int);
	sigaction(SIGQUIT, NULL, &g_original_quit);
	sigaction(SIGTSTP, NULL, &g_original_tstp);
	
	// Set up SIGINT (CTRL-C) to display a new prompt on a new line
	sigaction(SIGINT, &sa, NULL);
	
	// Make SIGQUIT (CTRL-\) behave the same as SIGINT
	sigaction(SIGQUIT, &sa, NULL);
	
	// Ignore SIGTSTP (CTRL-Z) to prevent suspending the shell
	sigaction(SIGTSTP, &sa, NULL);
}

// Ignore signals (for parent shell when child shell is running)
void	ignore_signals(void)
{
	struct sigaction	sa;

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

// Restore signal handling (for parent shell when child shell exits)
void	restore_signals(void)
{
	setup_signals();
} 