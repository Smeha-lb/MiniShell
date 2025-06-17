#include "../includes/minishell.h"

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

static char	*create_env_string(const char *name, const char *value)
{
	char	*new_env;

	new_env = ft_strjoin(name, "=");
	new_env = ft_strjoin_free(new_env, value);
	return (new_env);
}

void	set_env_var(t_shell *shell, const char *name, const char *value)
{
	int		index;
	char	*new_env;

	index = find_env_var(shell, name);
	new_env = create_env_string(name, value);
	if (index != -1)
	{
		free(shell->env[index]);
		shell->env[index] = new_env;
	}
	else
		add_env_var(shell, new_env);
}

static char	**create_env_without_var(t_shell *shell, int index, int size)
{
	char	**new_env_array;
	int		i;
	int		j;

	new_env_array = (char **)malloc(size * sizeof(char *));
	if (!new_env_array)
		return (NULL);
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
	return (new_env_array);
}

void	unset_env_var(t_shell *shell, const char *name)
{
	int		index;
	int		size;
	char	**new_env_array;

	index = find_env_var(shell, name);
	if (index == -1)
		return ;
	size = count_array(shell->env);
	new_env_array = create_env_without_var(shell, index, size);
	if (!new_env_array)
		return ;
	free(shell->env);
	shell->env = new_env_array;
}
