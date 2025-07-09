/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_vars.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:56 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:56 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	get_var_name_len(const char *str)
{
	int	i;

	i = 0;
	if (str[0] == '?')
		return (1);
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	return (i);
}

char	*get_var_value(t_shell *shell, const char *var_name, int name_len)
{
	char	*name;
	char	*value;

	if (name_len == 1 && var_name[0] == '?')
		return (ft_itoa(shell->exit_status));
	name = ft_substr(var_name, 0, name_len);
	if (!name)
		return (NULL);
	value = get_env_value(shell, name);
	free(name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

/**
 * Calculate size contribution of a variable expansion
 */
int	calculate_var_size(t_shell *shell, const char *str, int *i)
{
	int		var_name_len;
	char	*var_value;
	int		size;

	(*i)++;
	var_name_len = get_var_name_len(str + *i);
	var_value = get_var_value(shell, str + *i, var_name_len);
	size = 0;
	if (var_value)
	{
		size = ft_strlen(var_value);
		free(var_value);
	}
	*i += var_name_len;
	return (size);
}

/**
 * Check if character at position needs variable expansion
 */
int	needs_var_expansion(const char *str, int i)
{
	return (str[i] == '$' && !is_inside_single_quotes(str, i)
		&& str[i + 1] && (ft_isalnum(str[i + 1])
			|| str[i + 1] == '_' || str[i + 1] == '?'));
}

int	calculate_expanded_size(t_shell *shell, const char *str)
{
	int		i;
	int		size;

	if (!str)
		return (0);
	i = 0;
	size = 0;
	while (str[i])
	{
		if (needs_var_expansion(str, i))
		{
			size += calculate_var_size(shell, str, &i);
		}
		else
		{
			size++;
			i++;
		}
	}
	return (size);
}
