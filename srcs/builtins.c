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

int	execute_builtin(t_shell *shell, t_command *cmd)
{
	char	*builtin;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	builtin = cmd->args[0];
	if (ft_strcmp(builtin, "echo") == 0)
		return (builtin_echo(cmd));
	else if (ft_strcmp(builtin, "cd") == 0)
		return (builtin_cd(shell, cmd));
	else if (ft_strcmp(builtin, "pwd") == 0)
		return (builtin_pwd());
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

int	builtin_env(t_shell *shell)
{
	int	i;

	i = 0;
	while (shell->env[i])
	{
		ft_putendl_fd(shell->env[i], STDOUT_FILENO);
		i++;
	}
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
