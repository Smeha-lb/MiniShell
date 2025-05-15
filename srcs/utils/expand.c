/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Check if character is valid for variable name
 */
static int	is_valid_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

/**
 * Extract variable name
 */
static char	*extract_var_name(char *str, int *i)
{
	int		start;
	char	*var_name;

	start = *i;
	while (str[*i] && is_valid_var_char(str[*i]))
		(*i)++;
	var_name = ft_substr(str, start, *i - start);
	return (var_name);
}

/**
 * Expand a single variable
 */
static char	*expand_var(t_minishell *shell, char *str, int *i)
{
	char	*var_name;
	char	*var_value;
	char	*result;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->last_exit_status));
	}
	if (!str[*i] || !is_valid_var_char(str[*i]))
		return (ft_strdup("$"));
	var_name = extract_var_name(str, i);
	if (!var_name)
		return (NULL);
	var_value = get_env_value(shell->env, var_name);
	free(var_name);
	if (!var_value)
		return (ft_strdup(""));
	result = ft_strdup(var_value);
	return (result);
}

/**
 * Remove quotes from a string, handling nested and escape sequences
 */
char	*remove_quotes(char *str)
{
	int		i;
	int		j;
	char	*result;
	int		in_quotes;
	char	quote_type;

	if (!str)
		return (NULL);
	result = (char *)malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_quotes = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"') && (!in_quotes || quote_type == str[i]))
		{
			if (!in_quotes)
			{
				in_quotes = 1;
				quote_type = str[i];
			}
			else
				in_quotes = 0;
			i++;
		}
		else
		{
			result[j++] = str[i++];
		}
	}
	result[j] = '\0';
	return (result);
}

/**
 * Expand variables in a string
 */
char	*expand_variables(t_minishell *shell, char *str)
{
	int		i;
	char	*result;
	char	*tmp;
	char	*var_value;
	char	*substr;

	i = 0;
	result = ft_strdup("");
	while (str[i])
	{
		if (str[i] == '$')
		{
			var_value = expand_var(shell, str, &i);
			tmp = ft_strjoin(result, var_value);
			free(result);
			free(var_value);
			result = tmp;
		}
		else
		{
			substr = ft_substr(str, i, 1);
			tmp = ft_strjoin(result, substr);
			free(result);
			free(substr);
			result = tmp;
			i++;
		}
	}
	return (result);
} 