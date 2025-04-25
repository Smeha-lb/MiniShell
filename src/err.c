/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   err.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 15:51:55 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/25 14:55:46 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// TODO: Free stuff as needed.
// ! this might double free user_input?
void	exit_on_err(int exit_code, char *str)
{
	ft_dprintf(STDERR_FILENO, "%s - %s", str, strerror(exit_code));
	signal_handler.exit_code = exit_code;
	exit(EXIT_FAILURE);
}