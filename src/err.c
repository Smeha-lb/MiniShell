/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   err.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:51:55 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/24 12:52:33 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

void	print_error(char *str)
{
	ft_putstr_fd("🛑 ", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

// TODO: Free stuff as needed.
// ! this might double free user_input?
void	clean_exit(t_minishell *minishell, int exit_code, char *str)
{
	if (str)
		print_error(str);
	free(&minishell->user_input);
	rl_clear_history();
}