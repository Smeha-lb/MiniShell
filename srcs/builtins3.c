#include "../includes/minishell.h"

char	*is_valid_export_arg(char *arg)
{
	char	*equal_sign;

	equal_sign = ft_strchr(arg, '=');
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

void	display_env_var(char *var)
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

	sorted_env = prepare_sorted_env(shell, &env_size);
	if (!sorted_env)
		return (1);
	display_and_cleanup_env(sorted_env, env_size);
	return (0);
}

int	builtin_export(t_shell *shell, t_command *cmd)
{
	if (!cmd->args[1])
		return (display_sorted_env(shell));
	return (process_export_args(shell, cmd));
}
