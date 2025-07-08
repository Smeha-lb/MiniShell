#include "../includes/minishell.h"

/**
 * Update quote state for a character
 */
void	update_quote_state(char c, int prev_escaped,
	int *in_single_quotes, int *in_double_quotes)
{
	if (c == '\'' && !*in_double_quotes && !prev_escaped)
		*in_single_quotes = !*in_single_quotes;
	else if (c == '\"' && !*in_single_quotes && !prev_escaped)
		*in_double_quotes = !*in_double_quotes;
}

/**
 * Check if character at previous position is escape character
 */
int	is_escaped(const char *str, int i)
{
	return (i > 0 && str[i - 1] == '\\');
}

int	is_inside_single_quotes(const char *str, int pos)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	if (!str)
		return (0);
	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (i < pos && str[i])
	{
		update_quote_state(str[i], is_escaped(str, i),
			&in_single_quotes, &in_double_quotes);
		i++;
	}
	return (in_single_quotes);
}

int	is_inside_double_quotes(const char *str, int pos)
{
	int	i;
	int	in_double_quotes;
	int	in_single_quotes;

	if (!str)
		return (0);
	i = 0;
	in_double_quotes = 0;
	in_single_quotes = 0;
	while (i < pos && str[i])
	{
		update_quote_state(str[i], is_escaped(str, i),
			&in_single_quotes, &in_double_quotes);
		i++;
	}
	return (in_double_quotes);
}
