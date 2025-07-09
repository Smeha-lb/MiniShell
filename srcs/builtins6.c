/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins6.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:58:50 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 15:58:50 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_exit(t_shell *shell, t_command *cmd)
{
	int	exit_status;

	if (cmd->args[1] && cmd->args[2])
	{
		ft_putendl_fd("exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	ft_putendl_fd("exit", STDOUT_FILENO);
	if (cmd->args[1])
	{
		if (ft_isdigit_str(cmd->args[1]))
		{
			exit_status = ft_atoi(cmd->args[1]);
			shell->exit_status = exit_status;
		}
		else
		{
			ft_putendl_fd("exit: numeric argument required", STDERR_FILENO);
			shell->exit_status = 2;
		}
	}
	else
		exit_status = shell->exit_status;
	shell->running = 0;
	return (shell->exit_status);
}
