/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Check if a character is valid for a variable name
 */
static int	is_valid_var_char(char c, int first)
{
	if (first)
		return (ft_isalpha(c) || c == '_');
	return (ft_isalnum(c) || c == '_');
}

/**
 * Check if a variable name is valid
 */
static int	is_valid_var_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!is_valid_var_char(name[0], 1))
		return (0);
	i = 1;
	while (name[i] && name[i] != '=')
	{
		if (!is_valid_var_char(name[i], 0))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Print all environment variables in sorted order
 */
static void	print_sorted_env(char **env)
{
	char	**sorted;
	char	*tmp;
	int		i;
	int		j;
	int		len;

	len = 0;
	while (env[len])
		len++;
	sorted = (char **)malloc(sizeof(char *) * (len + 1));
	if (!sorted)
		return ;
	i = 0;
	while (i < len)
	{
		sorted[i] = env[i];
		i++;
	}
	sorted[i] = NULL;
	i = 0;
	while (i < len - 1)
	{
		j = 0;
		while (j < len - i - 1)
		{
			if (ft_strcmp(sorted[j], sorted[j + 1]) > 0)
			{
				tmp = sorted[j];
				sorted[j] = sorted[j + 1];
				sorted[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
	i = 0;
	while (sorted[i])
	{
		ft_putstr_fd("declare -x ", 1);
		j = 0;
		while (sorted[i][j] && sorted[i][j] != '=')
			j++;
		write(1, sorted[i], j);
		if (sorted[i][j] == '=')
		{
			ft_putstr_fd("=\"", 1);
			ft_putstr_fd(sorted[i] + j + 1, 1);
			ft_putstr_fd("\"", 1);
		}
		ft_putstr_fd("\n", 1);
		i++;
	}
	free(sorted);
}

/**
 * Parse a variable assignment
 */
static int	parse_var_assignment(char *arg, char **name, char **value)
{
	int		i;
	char	*equal_sign;

	equal_sign = ft_strchr(arg, '=');
	if (!equal_sign)
	{
		*name = ft_strdup(arg);
		*value = NULL;
	}
	else
	{
		i = equal_sign - arg;
		*name = ft_substr(arg, 0, i);
		*value = ft_strdup(equal_sign + 1);
	}
	if (!*name || (equal_sign && !*value))
	{
		free(*name);
		if (*value)
			free(*value);
		return (0);
	}
	return (1);
}

/**
 * Built-in export command
 */
int	ft_export(t_minishell *shell, char **args)
{
	int		i;
	char	*name;
	char	*value;
	int		ret;

	ret = 0;
	if (!args[1])
	{
		print_sorted_env(shell->env);
		return (0);
	}
	i = 1;
	while (args[i])
	{
		if (!parse_var_assignment(args[i], &name, &value))
		{
			print_error("export", "memory allocation error");
			return (1);
		}
		if (!is_valid_var_name(name))
		{
			print_error("export", "not a valid identifier");
			ret = 1;
		}
		else
			set_env_var(&shell->env, name, value);
		free(name);
		if (value)
			free(value);
		i++;
	}
	return (ret);
} 