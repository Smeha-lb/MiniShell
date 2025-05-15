/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
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
	while (name[i])
	{
		if (!is_valid_var_char(name[i], 0))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Built-in unset command
 */
int	ft_unset(t_minishell *shell, char **args)
{
	int	i;
	int	ret;

	ret = 0;
	if (!args[1])
		return (0);
	i = 1;
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			print_error("unset", "not a valid identifier");
			ret = 1;
		}
		else
			unset_env_var(&shell->env, args[i]);
		i++;
	}
	return (ret);
} 