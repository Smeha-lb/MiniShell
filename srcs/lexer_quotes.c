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

// Process content based on the OUTERMOST quote type
char	*process_quote_content(t_shell *shell, char *content, char quote_type)
{
	if (quote_type == '\'')
		return (ft_strdup(content));
	return (expand_variables(shell, content, 1));
}

// Handle a quoted section - the outermost quote determines all behavior
int	handle_quote_section(t_temp_var_data data, t_shell *shell)
{
	char	quote_type;
	int		end_pos;
	char	*content;
	char	*processed;
	int		content_len;
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
	*data.i = end_pos + 1;
	return (quote_flag);
}

// Legacy functions for compatibility
int	handle_quotes(t_temp_var_data data, t_shell *shell)
{
	return (handle_quote_section(data, shell));
}
