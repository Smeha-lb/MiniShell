#include "../includes/minishell.h"

static char	**setup_wildcard_expansion(t_match_data *data, const char *pattern)
{
	split_path(pattern, &data->dir_part, &data->file_part);
	data->matches = init_matches(data->capacity);
	if (!data->matches)
	{
		free(data->dir_part);
		free(data->file_part);
		return (NULL);
	}
	return (data->matches);
}

char	**expand_wildcards(const char *pattern)
{
	t_match_data	data;

	data.capacity = 10;
	data.matches = NULL;
	if (!has_wildcards(pattern))
		return (handle_no_wildcards(pattern));
	data.matches = setup_wildcard_expansion(&data, pattern);
	if (!data.matches)
		return (NULL);
	data.dir = opendir(data.dir_part);
	if (!data.dir)
		return (handle_dir_error(data.dir_part, data.file_part));
	return (process_wildcard_matches(&data, pattern));
}

static int	calculate_total_length(char **matches)
{
	int	i;
	int	total_len;

	total_len = 0;
	i = 0;
	while (matches[i])
	{
		total_len += ft_strlen(matches[i]);
		if (matches[i + 1])
			total_len++;
		i++;
	}
	return (total_len);
}

static void	copy_matches_to_result(char **matches, char *result)
{
	int	i;
	int	pos;

	pos = 0;
	i = 0;
	while (matches[i])
	{
		ft_strlcpy(result + pos, matches[i], ft_strlen(matches[i]) + 1);
		pos += ft_strlen(matches[i]);
		if (matches[i + 1])
			result[pos++] = ' ';
		i++;
	}
}

char	*join_expanded_wildcards(char **matches)
{
	int		total_len;
	char	*result;

	if (!matches || !matches[0])
		return (ft_strdup(""));
	total_len = calculate_total_length(matches);
	result = (char *)malloc((total_len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	copy_matches_to_result(matches, result);
	result[total_len] = '\0';
	return (result);
}
