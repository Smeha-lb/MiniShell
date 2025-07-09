/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:10:24 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:10:25 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	is_variable(char c1, char c2)
{
	return (c1 == '$' && c2 && (ft_isalnum(c2) || c2 == '_'
			|| c2 == '?') && c2 != '\"');
}

/**
 * Process a single character during expansion
 */
void	process_expansion_char(t_shell *shell, t_var_expansion_data *data)
{
	if (is_variable(data->str[*data->i], data->str[*data->i + 1]))
	{
		if (data->in_quotes || !is_inside_single_quotes(data->str, *data->i))
		{
			copy_variable(shell, (char *)data->str,
				data->i, data->expanded + *data->j);
			*data->j += ft_strlen(data->expanded + *data->j);
		}
		else
			process_char((char *)data->str, data->i, data->expanded, data->j);
	}
	else
		process_char((char *)data->str, data->i, data->expanded, data->j);
}

char	*expand_variables(t_shell *shell, char *str, int in_quotes)
{
	int						i;
	int						j;
	char					*expanded;
	t_var_expansion_data	data;

	if (!str)
		return (NULL);
	expanded = allocate_expanded_string(shell, str);
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	data.str = str;
	data.i = &i;
	data.expanded = expanded;
	data.j = &j;
	data.in_quotes = in_quotes;
	while (str[i])
	{
		process_expansion_char(shell, &data);
	}
	expanded[j] = '\0';
	return (expanded);
}

char	*allocate_expanded_string(t_shell *shell, char *str)
{
	char	*expanded;

	expanded = (char *)malloc((calculate_expanded_length(shell, str) + 1)
			*sizeof(char));
	return (expanded);
}

void	process_char(char *str, int *i, char *expanded, int *j)
{
	expanded[*j] = str[*i];
	(*j)++;
	(*i)++;
}
