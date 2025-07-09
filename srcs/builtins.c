/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:58:30 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 15:58:30 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*get_cd_path(t_shell *shell, char *arg)
{
	char	*path;

	if (!arg)
	{
		path = get_env_value(shell, "HOME");
		if (!path)
		{
			ft_putendl_fd("cd: HOME not set", STDERR_FILENO);
			return (NULL);
		}
		return (path);
	}
	if (ft_strcmp(arg, "~") == 0)
	{
		path = get_env_value(shell, "HOME");
		if (!path)
		{
			ft_putendl_fd("cd: HOME not set", STDERR_FILENO);
			return (NULL);
		}
		return (path);
	}
	return (arg);
}

/**
 * Route builtin command to appropriate function
 */
int	route_builtin_command(t_shell *shell, t_command *cmd, char *builtin)
{
	if (ft_strcmp(builtin, "echo") == 0)
		return (builtin_echo(cmd));
	else if (ft_strcmp(builtin, "cd") == 0)
		return (builtin_cd(shell, cmd));
	else if (ft_strcmp(builtin, "pwd") == 0)
		return (builtin_pwd(cmd));
	else if (ft_strcmp(builtin, "export") == 0)
		return (builtin_export(shell, cmd));
	else if (ft_strcmp(builtin, "unset") == 0)
		return (builtin_unset(shell, cmd));
	else if (ft_strcmp(builtin, "env") == 0)
		return (builtin_env(shell));
	else if (ft_strcmp(builtin, "exit") == 0)
		return (builtin_exit(shell, cmd));
	return (1);
}

// Execute builtin with variable expansion
// Create a temporary command with expanded arguments
int	execute_builtin_with_expanded_args(t_shell *shell,
		t_command *cmd, char **expanded_args)
{
	char		*builtin;
	t_command	temp_cmd;

	if (!expanded_args || !expanded_args[0])
		return (1);
	temp_cmd = *cmd;
	temp_cmd.args = expanded_args;
	builtin = expanded_args[0];
	return (route_builtin_command(shell, &temp_cmd, builtin));
}

// The args are already expanded in execute_current_command
int	execute_builtin(t_shell *shell, t_command *cmd)
{
	int		result;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	result = execute_builtin_with_expanded_args(shell, cmd, cmd->args);
	return (result);
}

// Use the current STDOUT_FILENO which might be redirected to a pipe
// Write directly to the file descriptor to ensure proper redirection
// Ensure all output is flushed to the pipe
int	builtin_env(t_shell *shell)
{
	int	i;
	int	fd;

	fd = STDOUT_FILENO;
	i = 0;
	while (shell->env[i])
	{
		if (write(fd, shell->env[i], ft_strlen(shell->env[i])) < 0)
			return (1);
		if (write(fd, "\n", 1) < 0)
			return (1);
		i++;
	}
	return (0);
}
