/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:12 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/11 13:41:43 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// line is a pointer to a string, not a 2d array
// we love the C type system don't we folks?

// ? we're saying here that we don't care about interrupts,
// ?  but we do care about other errors.
bool	get_input(char **line, char *input, bool update_hist, bool is_tty)
{
	char	*temp;

	errno = OK;
	if (is_tty)
		*line = readline(input);
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
int	main(int argc, char **argv, char **envp)
{
	char	*input;
	int		fd_in;
	int		fd_out;

	(void)argc;
	(void)argv;
	shell_init(envp, &fd_in, &fd_out); // <- handles env variables
	while (true)
	{
		// // TODO: Handle Interrupts: Ctrl+D etc (readline already does this?)
		// TODO: Capture heredoc if it exists
		// ? 1) get line
		input = readline("minishell$>");
		if (!input)
			break ;
		// TODO: Add history - see {using|add|free}_history
		// ? 2> get tokens
		parse_input(input);
		// TODO: Create syntax tree
		// ? 3) lexing
		// ? 4) parsing
		// ? 5) evaluation
	}

	return (EXIT_SUCCESS);
}
