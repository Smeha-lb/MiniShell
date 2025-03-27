/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:12 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/27 14:52:11 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// TODO: create a function to turn env to a linked list
int main(int argc, char **argv, char **envp) {
	char	*input;
	int		fd_in;
	int		fd_out;

	(void)argc;
	(void)argv;
	shell_init(envp, &fd_in, &fd_out); // <- handles env variables
	while (true)
	{
		// TODO: Handle Interrupts: Ctrl+D etc
		// TODO: Capture heredoc if it exists
		// ? 1) get line
		input = readline("minishell$>");
		if (!input)
			break ;
		// TODO: Add history
		// ? 2> get tokens
		// TODO: Create syntax tree
		// ? 3) lexing
		// ? 4) parsing
		// ? 5) evaluation
	}

	return (EXIT_SUCCESS);
}
