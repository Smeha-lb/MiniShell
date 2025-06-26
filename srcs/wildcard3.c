#include "../includes/minishell.h"

static int	check_asterisk_match(const char *pattern, const char *filename)
{
	if (does_pattern_match(pattern + 1, filename))
		return (1);
	return (0);
}

static int	handle_asterisk(const char *pattern, const char *filename)
{
	while (*(pattern + 1) == '*')
		pattern++;
	if (*(pattern + 1) == '\0')
		return (1);
	while (*filename)
	{
		if (check_asterisk_match(pattern, filename))
			return (1);
		filename++;
	}
	return (does_pattern_match(pattern + 1, filename));
}

static int	handle_bracket(const char *pattern, const char *filename)
{
	const char	*closing_bracket;

	closing_bracket = ft_strchr(pattern, ']');
	if (!closing_bracket)
		return (*pattern == *filename
			&& does_pattern_match(pattern + 1, filename + 1));
	if (!is_in_char_class(*filename, pattern))
		return (0);
	return (does_pattern_match(closing_bracket + 1, filename + 1));
}

// Skip . and .. directories
int	does_pattern_match(const char *pattern, const char *filename)
{
	if ((ft_strcmp(filename, ".") == 0 || ft_strcmp(filename, "..") == 0)
		&& (pattern[0] == '.' && pattern[1] == '*' && pattern[2] == '\0'))
		return (0);
	if (filename[0] == '.' && pattern[0] != '.')
		return (0);
	if (*pattern == '\0' && *filename == '\0')
		return (1);
	if (*pattern == '*')
		return (handle_asterisk(pattern, filename));
	if (*pattern == '[')
		return (handle_bracket(pattern, filename));
	if (*pattern == '?' && *filename != '\0')
		return (does_pattern_match(pattern + 1, filename + 1));
	if (*pattern == *filename)
		return (does_pattern_match(pattern + 1, filename + 1));
	return (0);
}

void	swap_matches(char **matches, int i, int j)
{
	char	*temp;

	temp = matches[i];
	matches[i] = matches[j];
	matches[j] = temp;
}
