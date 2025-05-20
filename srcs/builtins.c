#include "../includes/minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0 ||
			ft_strcmp(cmd, "cd") == 0 ||
			ft_strcmp(cmd, "pwd") == 0 ||
			ft_strcmp(cmd, "export") == 0 ||
			ft_strcmp(cmd, "unset") == 0 ||
			ft_strcmp(cmd, "env") == 0 ||
			ft_strcmp(cmd, "exit") == 0);
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

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	n_flag;

	n_flag = 0;
	i = 1;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		n_flag = 1;
		i++;
	}
	while (cmd->args[i])
	{
		ft_putstr_fd(cmd->args[i], STDOUT_FILENO);
		if (cmd->args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

int	builtin_cd(t_shell *shell, t_command *cmd)
{
	char	*path;
	char	cwd[1024];

	if (!cmd->args[1])
	{
		path = get_env_value(shell, "HOME");
		if (!path)
		{
			ft_putendl_fd("cd: HOME not set", STDERR_FILENO);
			return (1);
		}
	}
	else
		path = cmd->args[1];
	
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(shell, "OLDPWD", cwd);
	
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		return (1);
	}
	
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(shell, "PWD", cwd);
	
	return (0);
}

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		ft_putendl_fd(cwd, STDOUT_FILENO);
		return (0);
	}
	else
	{
		print_error("pwd", NULL, strerror(errno));
		return (1);
	}
}

int	builtin_export(t_shell *shell, t_command *cmd)
{
	int		i;
	int		j;
	char	*key;
	char	*value;
	char	*equal_sign;
	char	**sorted_env;
	int		env_size;

	if (!cmd->args[1])
	{
		// When no arguments, display all variables in sorted order with declare -x prefix
		env_size = count_array(shell->env);
		sorted_env = (char **)malloc((env_size + 1) * sizeof(char *));
		if (!sorted_env)
			return (1);
		
		// Copy environment variables
		for (i = 0; i < env_size; i++)
			sorted_env[i] = ft_strdup(shell->env[i]);
		sorted_env[i] = NULL;
		
		// Sort the environment variables alphabetically
		for (i = 0; i < env_size - 1; i++)
		{
			for (j = i + 1; j < env_size; j++)
			{
				if (ft_strcmp(sorted_env[i], sorted_env[j]) > 0)
				{
					char *temp = sorted_env[i];
					sorted_env[i] = sorted_env[j];
					sorted_env[j] = temp;
				}
			}
		}
		
		// Display environment variables with declare -x prefix
		for (i = 0; i < env_size; i++)
		{
			equal_sign = ft_strchr(sorted_env[i], '=');
			if (equal_sign)
			{
				*equal_sign = '\0';
				ft_putstr_fd("declare -x ", STDOUT_FILENO);
				ft_putstr_fd(sorted_env[i], STDOUT_FILENO);
				ft_putstr_fd("=\"", STDOUT_FILENO);
				ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
				ft_putendl_fd("\"", STDOUT_FILENO);
				*equal_sign = '='; // Restore the string to its original state
			}
			else
			{
				ft_putstr_fd("declare -x ", STDOUT_FILENO);
				ft_putendl_fd(sorted_env[i], STDOUT_FILENO);
			}
		}
		
		// Free the sorted array
		for (i = 0; i < env_size; i++)
			free(sorted_env[i]);
		free(sorted_env);
		
		return (0);
	}
	
	i = 1;
	while (cmd->args[i])
	{
		equal_sign = ft_strchr(cmd->args[i], '=');
		if (equal_sign)
		{
			key = ft_substr(cmd->args[i], 0, equal_sign - cmd->args[i]);
			value = ft_strdup(equal_sign + 1);
			set_env_var(shell, key, value);
			free(key);
			free(value);
		}
		else
		{
			// If no '=' is present, just mark the variable for export without changing its value
			value = get_env_value(shell, cmd->args[i]);
			if (value)
				set_env_var(shell, cmd->args[i], value);
			else
				set_env_var(shell, cmd->args[i], "");
		}
		i++;
	}
	return (0);
}

int	builtin_unset(t_shell *shell, t_command *cmd)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		unset_env_var(shell, cmd->args[i]);
		i++;
	}
	return (0);
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