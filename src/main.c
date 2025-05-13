#include "../inc/minishell.h"
#include <signal.h>

t_signal_handler	signal_handler;

static pid_t	get_pid(void)
{
	DIR				*dir;
	struct dirent	*file;
	pid_t			pid;

	pid = 0;
	dir = opendir("/proc/self/task");
	if (dir == NULL)
		return (pid);
	file = readdir(dir);
	while (file != NULL && pid == 0)
	{
		pid = ft_atoi(file->d_name);
		file = readdir(dir);
	}
	closedir(dir);
	return (pid);
}

static void	handle_sigint(int sig)
{
	(void)sig;
	ft_printf("\n");
}

static void	handle_sigquit(int sig)
{
	(void)sig;
	ft_printf("\n");
}

static void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = handle_sigint;
	sa_quit.sa_handler = handle_sigquit;
	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

static void	shell_init(t_minishell *minishell)
{
	minishell->is_tty = isatty(STDIN_FILENO);
	minishell->shell_pid = get_pid();
	minishell->subshell_pid = -1;
	minishell->exit_status = OK;
	setup_signals();
	// TODO: Clean on exit: Clear lists, free cmd table, clear history
}

// line is a pointer to a string, not a 2d array
// we love the C type system don't we folks?

// ? we're saying here that we don't care about interrupts,
// ?  but we do care about other errors.
// * `return (free(temp), false)` -> this works because:
// * 1 - return takes any expression
// * 2 - comma is an operator that takes any number of expressions, evaluates
// * them, then discards everything but the rightmost expression.

static bool	get_input(char **line, bool update_hist, bool is_tty)
{
	char	*temp;

	errno = OK;
	*line = NULL;  // Initialize line to NULL
	if (!is_tty)  // Changed from if (is_tty) to if (!is_tty)
		*line = readline("minishell%%> ");
	else
	{
		temp = get_next_line(STDIN_FILENO);
		if (errno != OK)
			return (free(temp), false);
		if (temp)
		{
			*line = ft_strtrim(temp, "\n");
			free(temp);
		}
	}
	if (errno == EINTR)
		errno = OK;
	else if (errno != OK)
		return (false);
	if (update_hist && *line && **line)
		add_history(*line);
	return (errno == OK);
}

// TODO: create a function to turn env to a linked list
// ! should main take envp?
int	main(void)
{
	t_minishell	minishell;
	t_astree	*ast;

	minishell.user_input = NULL;
	printf("Starting minishell...\n");  // Debug print
	shell_init(&minishell);
	while (1)
	{
		printf("Waiting for input...\n");  // Debug print
		if (!get_input(&minishell.user_input, true, minishell.is_tty))
			continue;
		if (!minishell.user_input)
		{
			printf("exit\n");
			break;
		}
		printf("Got input: %s\n", minishell.user_input);  // Debug print
		ast = generate_astree(minishell.user_input);
		printf("Got to this line\n");  // Debug print
		if (ast)
		{
			// execute ast
			free(ast);
		}
		else
			printf("Failed to generate AST\n");
	}
	return (0);
}
