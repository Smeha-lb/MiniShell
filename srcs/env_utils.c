#include "../includes/minishell.h"

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

int	find_env_var(t_shell *shell, const char *name)
{
	int		i;
	char	*env_name;

	i = 0;
	while (shell->env[i])
	{
		env_name = get_env_name(shell->env[i]);
		if (ft_strcmp(env_name, name) == 0)
		{
			free(env_name);
			return (i);
		}
		free(env_name);
		i++;
	}
	return (-1);
}

void	set_env_var(t_shell *shell, const char *name, const char *value)
{
	int		index;
	char	*new_env;
	char	**new_env_array;
	int		i;
	int		size;

	index = find_env_var(shell, name);
	new_env = ft_strjoin(name, "=");
	new_env = ft_strjoin_free(new_env, value);
	
	if (index != -1)
	{
		free(shell->env[index]);
		shell->env[index] = new_env;
	}
	else
	{
		size = count_array(shell->env);
		new_env_array = (char **)malloc((size + 2) * sizeof(char *));
		if (!new_env_array)
		{
			free(new_env);
			return;
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
}

void	unset_env_var(t_shell *shell, const char *name)
{
	int		index;
	int		i;
	int		j;
	int		size;
	char	**new_env_array;

	index = find_env_var(shell, name);
	if (index == -1)
		return;
	
	size = count_array(shell->env);
	new_env_array = (char **)malloc(size * sizeof(char *));
	if (!new_env_array)
		return;
	
	i = 0;
	j = 0;
	while (i < size)
	{
		if (i != index)
			new_env_array[j++] = shell->env[i];
		else
			free(shell->env[i]);
		i++;
	}
	new_env_array[j] = NULL;
	free(shell->env);
	shell->env = new_env_array;
} 