/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:59:05 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 15:59:05 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	setup_child_redirections(t_command *cmd)
{
	if (cmd->pipe_in != -1)
	{
		dup2(cmd->pipe_in, STDIN_FILENO);
		close(cmd->pipe_in);
	}
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
	}
}

void	handle_child_process(t_shell *shell, t_command *cmd, char *path)
{
	setup_child_redirections(cmd);
	if (execve(path, cmd->args, shell->env) == -1)
	{
		if (errno == ENOEXEC)
			print_error(cmd->args[0], NULL, "Permission denied");
		else
			print_error(cmd->args[0], NULL, strerror(errno));
		exit(127);
	}
}

int	fork_external_command(t_shell *shell, t_command *cmd, char *path)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, strerror(errno));
		free(path);
		return (-1);
	}
	else if (pid == 0)
		handle_child_process(shell, cmd, path);
	free(path);
	return (pid);
}

int	wait_for_external_command(pid_t pid)
{
	int	status;

	ignore_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (0);
}

int	execute_external_command(t_shell *shell, t_command *cmd)
{
	char	*path;
	pid_t	pid;
	char	**temp_cmd_args;

	if (!(*cmd->arg_quoted == 2) && !cmd->args[1])
	{
		temp_cmd_args = ft_split(cmd->args[0], ' ');
		free_cmd_args(cmd->args);
		free(cmd->arg_quoted);
		cmd->args = temp_cmd_args;
		cmd->arg_quoted = NULL;
	}
	path = find_command_path(shell, cmd->args[0]);
	if (!path)
	{
		print_error(cmd->args[0], NULL, "command not found");
		return (127);
	}
	pid = fork_external_command(shell, cmd, path);
	if (pid == -1)
		return (1);
	return (wait_for_external_command(pid));
}
