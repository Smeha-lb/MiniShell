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

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}

// Execute builtin with variable expansion
int	execute_builtin_with_expanded_args(t_shell *shell, t_command *cmd, char **expanded_args)
{
	char	*builtin;
	int		result;
	t_command temp_cmd;

	if (!expanded_args || !expanded_args[0])
		return (1);
	
	// Create a temporary command with expanded arguments
	temp_cmd = *cmd;
	temp_cmd.args = expanded_args;
	
	builtin = expanded_args[0];
	
	if (ft_strcmp(builtin, "echo") == 0)
		result = builtin_echo(&temp_cmd);
	else if (ft_strcmp(builtin, "cd") == 0)
		result = builtin_cd(shell, &temp_cmd);
	else if (ft_strcmp(builtin, "pwd") == 0)
		result = builtin_pwd(&temp_cmd);
	else if (ft_strcmp(builtin, "export") == 0)
		result = builtin_export(shell, &temp_cmd);
	else if (ft_strcmp(builtin, "unset") == 0)
		result = builtin_unset(shell, &temp_cmd);
	else if (ft_strcmp(builtin, "env") == 0)
		result = builtin_env(shell);
	else if (ft_strcmp(builtin, "exit") == 0)
		result = builtin_exit(shell, &temp_cmd);
	else
		result = 1;
	
	return (result);
}

int	execute_builtin(t_shell *shell, t_command *cmd)
{
	char	**expanded_args;
	int		result;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	
	// Expand variables in arguments
	expanded_args = expand_command_args(shell, cmd->args);
	if (!expanded_args)
		return (1);
	
	// Execute the builtin with expanded arguments
	result = execute_builtin_with_expanded_args(shell, cmd, expanded_args);
	
	// Free the expanded arguments
	free_expanded_args(expanded_args);
	
	return (result);
}

int	builtin_env(t_shell *shell)
{
	int	i;
	int	fd;

	// Use the current STDOUT_FILENO which might be redirected to a pipe
	fd = STDOUT_FILENO;
	
	i = 0;
	while (shell->env[i])
	{
		// Write directly to the file descriptor to ensure proper redirection
		if (write(fd, shell->env[i], ft_strlen(shell->env[i])) < 0)
			return (1);
		if (write(fd, "\n", 1) < 0)
			return (1);
		i++;
	}
	
	// Ensure all output is flushed to the pipe
	fflush(stdout);
	return (0);
}

int	builtin_exit(t_shell *shell, t_command *cmd)
{
	int	exit_status;

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
	shell->running = 0;
	return (shell->exit_status);
}
