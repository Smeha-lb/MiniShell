#include "../includes/minishell.h"

// Find the matching closing quote, respecting nesting rules
int	find_matching_quote(char *input, int start, char quote_type)
{
	int	i;

	i = start + 1;
	while (input[i] && input[i] != quote_type)
		i++;
	if (input[i] == quote_type)
		return (i);
	return (-1);
}

// Expand variables without treating inner quotes as quote delimiters
char	*expand_variables_no_inner_quotes(t_shell *shell, const char *str)
{
	int		i;
	int		j;
	char	*expanded;

	if (!str)
		return (NULL);
	expanded = (char *)malloc((calculate_expanded_size_no_quotes
				(shell, str) + 1) * sizeof(char));
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
		{
			copy_var_to_expanded(shell, str, &i, expanded, &j);
		}
		else
			expanded[j++] = str[i++];
	}
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

/**
 * Process and copy quote content to word buffer
 */
int	process_and_copy_quote_content(t_temp_var_data data, t_shell *shell,
	char quote_type, int end_pos)
{
	char	*content;
	char	*processed;
	int		content_len;

	content = ft_substr(data.input, *data.i + 1, end_pos - *data.i - 1);
	if (!content)
		return (-1);
	processed = process_quote_content(shell, content, quote_type);
	free(content);
	if (!processed)
		return (-1);
	content_len = ft_strlen(processed);
	ft_strlcpy(*data.word + *data.j, processed, content_len + 1);
	*data.j += content_len;
	free(processed);
	return (0);
}

// Handle a quoted section - the outermost quote determines all behavior
int	handle_quote_section(t_temp_var_data data, t_shell *shell)
{
	char	quote_type;
	int		end_pos;
	int		quote_flag;

	quote_type = data.input[*data.i];
	if (quote_type == '\'')
		quote_flag = 1;
	else
		quote_flag = 2;
	end_pos = find_matching_quote(data.input, *data.i, quote_type);
	if (end_pos == -1)
	{
		ft_putendl_fd("Error: Unclosed quotes", 2);
		return (-1);
	}
	if (process_and_copy_quote_content(data, shell, quote_type, end_pos) == -1)
		return (-1);
	*data.i = end_pos + 1;
	return (quote_flag);
}

// Legacy functions for compatibility
int	handle_quotes(t_temp_var_data data, t_shell *shell)
{
	return (handle_quote_section(data, shell));
}
