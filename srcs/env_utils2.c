#include "../includes/minishell.h"

void	add_env_var(t_shell *shell, char *new_env)
{
	char	**new_env_array;
	int		i;
	int		size;

	size = count_array(shell->env);
	new_env_array = (char **)malloc((size + 2) * sizeof(char *));
	if (!new_env_array)
	{
		free(new_env);
		return ;
	}
	i = 0;
	while (i < size)
	{
		new_env_array[i] = shell->env[i];
		i++;
	}
	new_env_array[i] = new_env;
	new_env_array[i + 1] = NULL;
	free(shell->env);
	shell->env = new_env_array;
}

int	count_array(char **array)
{
	int	i;

	i = 0;
	while (array[i])
		i++;
	return (i);
}

char	**copy_env(char **env)
{
	char	**new_env;
	int		i;
	int		size;

	size = count_array(env);
	new_env = (char **)malloc((size + 1) * sizeof(char *));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < size)
	{
		new_env[i] = ft_strdup(env[i]);
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

char	*get_env_name(char *env_var)
{
	int		i;
	char	*name;

	i = 0;
	while (env_var[i] && env_var[i] != '=')
		i++;
	name = ft_substr(env_var, 0, i);
	return (name);
}

char	*get_env_value(t_shell *shell, const char *name)
{
	int		i;
	char	*env_name;
	char	*value;

	i = 0;
	while (shell->env[i])
	{
		env_name = get_env_name(shell->env[i]);
		if (ft_strcmp(env_name, name) == 0)
		{
			free(env_name);
			value = ft_strchr(shell->env[i], '=');
			if (value)
				return (value + 1);
			return (NULL);
		}
		free(env_name);
		i++;
	}
	return (NULL);
}
