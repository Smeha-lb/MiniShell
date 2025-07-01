#include "../includes/minishell.h"

char	*extract_quoted_content(char *input, int start, int end)
{
	return (ft_substr(input, start, end - start));
}

char	*process_quoted_content(t_shell *shell, char *content, char quote_type)
{
	// Prevent unused parameter warnings
	(void)shell;
	(void)quote_type;
	
	// Return the content as is, without expansion
	return (content);
}

int	copy_expanded_content(t_temp_var_data data, char *expanded)
{
	size_t	content_len;

	if (!expanded)
		return (1);
	content_len = ft_strlen(expanded);
	ft_strlcpy(*data.word + *data.j, expanded, content_len + 1);
	*data.j += content_len;
	free(expanded);
	return (0);
}

int	extract_and_process_quotes(t_temp_var_data data,
		t_shell *shell, char quote_type)
{
	int		start;
	char	*quoted_content;
	char	*expanded;

	start = *data.i;
	while (data.input[*data.i] && data.input[*data.i] != quote_type)
		(*data.i)++;
	if (!data.input[*data.i])
	{
		ft_putendl_fd("Error: Unclosed quotes", 2);
		return (1);
	}
	quoted_content = extract_quoted_content(data.input,
			start, *data.i);
	if (!quoted_content)
		return (1);
	expanded = process_quoted_content(shell, quoted_content, quote_type);
	if (copy_expanded_content(data, expanded))
		return (1);
	(*data.i)++;
	return (0);
}

int	handle_quotes(t_temp_var_data data, t_shell *shell)
{
	char	quote_type;

	quote_type = data.input[*data.i];
	(*data.i)++;
	if (extract_and_process_quotes(data, shell, quote_type))
		return (free(*data.word), 1);
	return (0);
}
