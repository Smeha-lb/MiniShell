/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:01:00 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:01:01 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*get_legacy_var_value(t_shell *shell, char *var_name, int name_len)
{
	char	*name;
	char	*value;
	char	*exit_status;

	if (name_len == 1 && var_name[0] == '?')
	{
		exit_status = ft_itoa(shell->exit_status);
		if (!exit_status)
			return (NULL);
		return (exit_status);
	}
	name = ft_substr(var_name, 0, name_len);
	if (!name)
		return (NULL);
	value = get_env_value(shell, name);
	free(name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

static int	process_variable(t_shell *shell, char *str, int *i,
	int *length)
{
	int		var_name_len;
	char	*var_value;

	(*i)++;
	var_name_len = get_var_name_len(str + *i);
	var_value = get_legacy_var_value(shell, str + *i, var_name_len);
	if (var_value)
	{
		*length += ft_strlen(var_value);
		free(var_value);
	}
	return (var_name_len);
}

int	calculate_expanded_length(t_shell *shell, char *str)
{
	int		i;
	int		length;

	i = 0;
	length = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
		{
			i += process_variable(shell, str, &i, &length);
		}
		else
		{
			length++;
			i++;
		}
	}
	return (length);
}

void	copy_variable(t_shell *shell, char *str, int *i, char *expanded)
{
	int		var_name_len;
	char	*var_value;
	int		j;

	(*i)++;
	var_name_len = get_var_name_len(str + *i);
	var_value = get_legacy_var_value(shell, str + *i, var_name_len);
	if (var_value)
	{
		j = 0;
		while (var_value[j])
		{
			expanded[j] = var_value[j];
			j++;
		}
		expanded[j] = '\0';
		free(var_value);
	}
	*i += var_name_len;
}
