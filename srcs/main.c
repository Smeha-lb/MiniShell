/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:11:26 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:11:27 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	g_signal_code = 0;

int	main(int argc, char **argv, char **env)
{
	t_shell	shell;

	(void)argv;
	if (argc != 1)
	{
		ft_putendl_fd("Error: minishell does not accept arguments", 2);
		return (1);
	}
	if (!isatty(STDIN_FILENO))
	{
		ft_putendl_fd("Error: minishell must be run in a terminal", 2);
		return (1);
	}
	shell_init(&shell, env);
	setup_signals();
	shell_loop(&shell);
	shell_cleanup(&shell);
	return (shell.exit_status);
}
