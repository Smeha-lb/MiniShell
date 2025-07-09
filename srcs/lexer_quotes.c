/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_quotes.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:10:40 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:10:41 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * Check if character position needs variable expansion (no inner quotes)
 */
int	needs_var_expansion_no_quotes(const char *str, int i)
{
	return (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1])
			|| str[i + 1] == '_' || str[i + 1] == '?'));
}

/**
 * Process expansion loop without considering inner quotes
 */
void	process_expansion_loop_no_quotes(t_shell *shell,
			t_var_expansion_data *data)
{
	while (data->str[*data->i])
	{
		if (needs_var_expansion_no_quotes(data->str, *data->i))
		{
			copy_var_to_expanded(shell, data);
		}
		else
			data->expanded[(*data->j)++] = data->str[(*data->i)++];
	}
}

// Expand variables without treating inner quotes as quote delimiters
char	*expand_variables_no_inner_quotes(t_shell *shell, const char *str)
{
	int						i;
	int						j;
	char					*expanded;
	t_var_expansion_data	data;

	if (!str)
		return (NULL);
	expanded = (char *)malloc((calculate_expanded_size_no_quotes
				(shell, str) + 1) * sizeof(char));
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	data.str = str;
	data.i = &i;
	data.expanded = expanded;
	data.j = &j;
	init_expansion_data(&data);
	process_expansion_loop_no_quotes(shell, &data);
	expanded[j] = '\0';
	return (expanded);
}

// Calculate size for expansion without considering inner quotes
int	calculate_expanded_size_no_quotes(t_shell *shell, const char *str)
{
	int		i;
	int		size;

	if (!str)
		return (0);
	i = 0;
	size = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
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

// Process content based on the OUTERMOST quote type
char	*process_quote_content(t_shell *shell, char *content, char quote_type)
{
	if (quote_type == '\'')
		return (ft_strdup(content));
	return (expand_variables_no_inner_quotes(shell, content));
}
