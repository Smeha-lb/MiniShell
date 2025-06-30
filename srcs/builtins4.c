#include "../includes/minishell.h"

void	process_export_arg(t_shell *shell, char *arg)
{
	char	*equal_sign;
	char	*key;
	char	*value;

	equal_sign = ft_strchr(arg, '=');
	if (equal_sign)
	{
		*equal_sign = '\0';
		key = ft_strdup(arg);
		*equal_sign = '=';
		value = ft_strdup(equal_sign + 1);
		set_env_var(shell, key, value);
		free(key);
		free(value);
	}
	else
	{
		value = get_env_value(shell, arg);
		if (value)
			set_env_var(shell, arg, value);
		else
			set_env_var(shell, arg, "");
	}
}

void	sort_env_vars(char **sorted_env, int env_size)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < env_size - 1)
	{
		j = i + 1;
		while (j < env_size)
		{
			if (ft_strcmp(sorted_env[i], sorted_env[j]) > 0)
			{
				temp = sorted_env[i];
				sorted_env[i] = sorted_env[j];
				sorted_env[j] = temp;
			}
			j++;
		}
		i++;
	}
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
