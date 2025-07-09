/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_subshell2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:44 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:44 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	handle_pipeline_subshell(t_shell *shell, t_command *cmd, int *exit_status)
{
	int	stdin_backup;
	int	stdout_backup;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	setup_pipeline_redirections(cmd);
	if (cmd->redirs && setup_redirections(shell, cmd) != 0)
		return (handle_redirection_failure(stdin_backup,
				stdout_backup, exit_status));
	*exit_status = execute_subshell(shell, cmd->subshell);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (1);
}

int	handle_subshell(t_shell *shell, t_command *cmd, int *exit_status)
{
	int	stdin_backup;
	int	stdout_backup;

	if (cmd->pipe_in != -1 || cmd->pipe_out != -1)
		return (handle_pipeline_subshell(shell, cmd, exit_status));
	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (cmd->redirs && setup_redirections(shell, cmd) != 0)
	{
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		*exit_status = 1;
		return (1);
	}
	*exit_status = execute_subshell(shell, cmd->subshell);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (1);
}

int	setup_builtin_redirects(t_shell *shell, t_command *cmd, int *in, int *out)
{
	*in = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	if (setup_redirections(shell, cmd) != 0)
		return (1);
	return (0);
}
