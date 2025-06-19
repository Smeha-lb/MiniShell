#include "../includes/minishell.h"

static char	*join_segments(char *result, char *segment)
{
	char	*joined;

	joined = ft_strjoin(result, segment);
	free(result);
	free(segment);
	return (joined);
}

static char	*handle_quoted_text(t_shell *shell, char *str, int *i, char *result)
{
	char	quote;
	char	*segment;

	quote = str[*i];
	segment = process_quoted_segment(shell, str, i, quote);
	if (!segment)
	{
		free(result);
		return (NULL);
	}
	result = join_segments(result, segment);
	(*i)++;
	return (result);
}

static char	*handle_unquoted_text(t_shell *shell, char *str,
	int *i, char *result)
{
	char	*segment;

	segment = process_unquoted_segment(shell, str, i);
	return (join_segments(result, segment));
}

static char	*process_string(t_shell *shell, char *str, int *i, char *result)
{
	if (is_quote(str[*i]))
	{
		result = handle_quoted_text(shell, str, i, result);
		if (!result)
			return (ft_strdup(str));
	}
	else
		result = handle_unquoted_text(shell, str, i, result);
	return (result);
}

char	*handle_expansion(t_shell *shell, char *str)
{
	int		i;
	char	*result;

	if (!str)
		return (NULL);
	result = init_result();
	if (!result)
		return (NULL);
	i = 0;
	while (str[i])
	{
		result = process_string(shell, str, &i, result);
	}
	return (result);
}
