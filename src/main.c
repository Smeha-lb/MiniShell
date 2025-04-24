/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:12 by moabdels          #+#    #+#             */
/*   Updated: 2025/05/05 17:08:55 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

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

static void	shell_init(t_minishell *minishell)
{
	minishell->is_tty = isatty(STDIN_FILENO);
	minishell->shell_pid = get_pid();
	minishell->subshell_pid = -1;
	minishell->exit_status = OK;
	// TODO: Signal handling.
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
	if (is_tty)
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
	bool		valid_input;
	t_minishell	minishell;
	t_astree	*root;

	shell_init(&minishell);
	while (true)
	{
		// // TODO: Handle Interrupts: Ctrl+D etc (readline already does this?)
		// TODO: Capture heredoc if it exists
		// TODO: Cleanup at each stage if errs happen
		valid_input = get_input(&minishell.user_input, true, minishell.is_tty);
		if (!valid_input)
			continue;
		signal_handler.is_executing = true;
		add_history(minishell.user_input);
		root = generate_astree(minishell.user_input);
		// ? 5) evaluation
	}
	return (EXIT_SUCCESS);
}
