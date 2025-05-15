/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Free a string array
 */
void	free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

/**
 * Get the length of an environment variable array
 */
static int	get_env_len(char **env)
{
	int	len;

	len = 0;
	while (env[len])
		len++;
	return (len);
}

/**
 * Initialize environment variables
 */
char	**init_env(char **envp)
{
	char	**env;
	int		i;

	if (!envp)
		return (NULL);
	i = get_env_len(envp);
	env = (char **)malloc(sizeof(char *) * (i + 1));
	if (!env)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		env[i] = ft_strdup(envp[i]);
		if (!env[i])
		{
			free_array(env);
			return (NULL);
		}
		i++;
	}
	env[i] = NULL;
	return (env);
}

/**
 * Get the value of an environment variable
 */
char	*get_env_value(char **env, const char *name)
{
	int		i;
	int		len;
	char	*value;

	if (!env || !name)
		return (NULL);
	i = 0;
	len = ft_strlen(name);
	while (env[i])
	{
		if (ft_strncmp(env[i], name, len) == 0 && env[i][len] == '=')
		{
			value = env[i] + len + 1;
			return (value);
		}
		i++;
	}
	return (NULL);
}

/**
 * Set an environment variable
 */
int	set_env_var(char ***env, const char *name, const char *value)
{
	char	**new_env;
	char	*var;
	char	*tmp;
	int		len;
	int		i;

	if (!env || !name)
		return (0);
	var = ft_strjoin(name, "=");
	if (!var)
		return (0);
	if (value)
	{
		tmp = ft_strjoin(var, value);
		free(var);
		if (!tmp)
			return (0);
		var = tmp;
	}
	i = 0;
	len = get_env_len(*env);
	while (i < len)
	{
		if (ft_strncmp((*env)[i], name, ft_strlen(name)) == 0 
			&& (*env)[i][ft_strlen(name)] == '=')
		{
			free((*env)[i]);
			(*env)[i] = var;
			return (1);
		}
		i++;
	}
	new_env = (char **)malloc(sizeof(char *) * (len + 2));
	if (!new_env)
	{
		free(var);
		return (0);
	}
	i = 0;
	while (i < len)
	{
		new_env[i] = (*env)[i];
		i++;
	}
	new_env[i] = var;
	new_env[i + 1] = NULL;
	free(*env);
	*env = new_env;
	return (1);
}

/**
 * Unset an environment variable
 */
int	unset_env_var(char ***env, const char *name)
{
	char	**new_env;
	int		len;
	int		i;
	int		j;

	if (!env || !name || !*env)
		return (0);
	len = get_env_len(*env);
	new_env = (char **)malloc(sizeof(char *) * len);
	if (!new_env)
		return (0);
	i = 0;
	j = 0;
	while ((*env)[i])
	{
		if (ft_strncmp((*env)[i], name, ft_strlen(name)) != 0 
			|| (*env)[i][ft_strlen(name)] != '=')
		{
			new_env[j] = (*env)[i];
			j++;
		}
		else
			free((*env)[i]);
		i++;
	}
	new_env[j] = NULL;
	free(*env);
	*env = new_env;
	return (1);
} 