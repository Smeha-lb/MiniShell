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
