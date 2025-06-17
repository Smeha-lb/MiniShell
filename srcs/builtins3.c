#include "../includes/minishell.h"

static void	copy_env_vars(char **env, char **sorted_env, int env_size)
{
	int	i;

	i = 0;
	while (i < env_size)
	{
		sorted_env[i] = ft_strdup(env[i]);
		i++;
	}
	sorted_env[i] = NULL;
}

static char	*is_valid_export_arg(char *arg)
{
	char	*equal_sign;

	equal_sign = ft_strchr(arg, '=');
	if (equal_sign && ft_strchr(equal_sign + 1, '=') != NULL)
		return ("not a valid identifier");
	if (equal_sign)
	{
		*equal_sign = '\0';
		if (!is_valid_var_name(arg))
		{
			*equal_sign = '=';
			return ("not a valid identifier");
		}
		*equal_sign = '=';
	}
	else if (!is_valid_var_name(arg))
		return ("not a valid identifier");
	return (NULL);
}

static void	display_env_var(char *var)
{
	char	*equal_sign;

	equal_sign = ft_strchr(var, '=');
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	if (equal_sign)
	{
		*equal_sign = '\0';
		ft_putstr_fd(var, STDOUT_FILENO);
		ft_putstr_fd("=\"", STDOUT_FILENO);
		ft_putstr_fd(equal_sign + 1, STDOUT_FILENO);
		ft_putendl_fd("\"", STDOUT_FILENO);
		*equal_sign = '=';
	}
	else
		ft_putendl_fd(var, STDOUT_FILENO);
}

static int	display_sorted_env(t_shell *shell)
{
	int		env_size;
	char	**sorted_env;
	int		i;

	env_size = count_array(shell->env);
	sorted_env = (char **)malloc((env_size + 1) * sizeof(char *));
	if (!sorted_env)
		return (1);
	copy_env_vars(shell->env, sorted_env, env_size);
	sort_env_vars(sorted_env, env_size);
	i = 0;
	while (i < env_size)
	{
		display_env_var(sorted_env[i]);
		i++;
	}
	i = 0;
	while (i < env_size)
	{
		free(sorted_env[i]);
		i++;
	}
	free(sorted_env);
	return (0);
}

int	builtin_export(t_shell *shell, t_command *cmd)
{
	int		i;
	int		status;
	char	*error;

	if (!cmd->args[1])
		return (display_sorted_env(shell));
	status = 0;
	i = 1;
	while (cmd->args[i])
	{
		error = is_valid_export_arg(cmd->args[i]);
		if (error)
		{
			ft_putstr_fd("export: `", STDERR_FILENO);
			ft_putstr_fd(cmd->args[i], STDERR_FILENO);
			ft_putstr_fd("': ", STDERR_FILENO);
			ft_putendl_fd(error, STDERR_FILENO);
			status = 1;
		}
		else
			process_export_arg(shell, cmd->args[i]);
		i++;
	}
	return (status);
}
