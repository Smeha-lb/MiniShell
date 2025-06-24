#include "../includes/minishell.h"

char	*extract_quoted_content(char *input, int start, int end)
{
	return (ft_substr(input, start, end - start));
}

char	*process_quoted_content(t_shell *shell, char *content, char quote_type)
{
	char	*expanded;

	if (quote_type == '\"')
	{
		expanded = expand_variables(shell, content, 1);
		free(content);
		return (expanded);
	}
	return (content);
}

int	handle_quotes(char *input, int *i, char **word, int *j, t_shell *shell)
{
	char	quote_type;
	int		start;
	char	*quoted_content;
	char	*expanded;
	size_t	content_len;

	quote_type = input[*i];
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	if (!input[*i])
	{
		ft_putendl_fd("Error: Unclosed quotes", 2);
		free(*word);
		return (1);
	}
	quoted_content = extract_quoted_content(input, start, *i);
	if (!quoted_content)
	{
		free(*word);
		return (1);
	}
	expanded = process_quoted_content(shell, quoted_content, quote_type);
	if (!expanded)
	{
		free(*word);
		return (1);
	}
	content_len = ft_strlen(expanded);
	ft_strlcpy(*word + *j, expanded, content_len + 1);
	*j += content_len;
	free(expanded);
	(*i)++;
	return (0);
}
