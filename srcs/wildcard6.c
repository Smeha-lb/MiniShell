#include "../includes/minishell.h"

static int	process_directory_entries(t_match_data *data)
{
	struct dirent	*entry;
	int				result;

	data->match_count = 0;
	entry = readdir(data->dir);
	while (entry != NULL)
	{
		if (does_pattern_match(data->file_part, entry->d_name))
		{
			result = add_matching_entry(data, entry->d_name);
			if (result == -1)
				return (-1);
		}
		entry = readdir(data->dir);
	}
	data->matches[data->match_count] = NULL;
	return (data->match_count);
}

char	**process_wildcard_matches(t_match_data *data, const char *pattern)
{
	int	match_count;

	match_count = process_directory_entries(data);
	if (match_count == -1)
		return (NULL);
	closedir(data->dir);
	free(data->dir_part);
	free(data->file_part);
	sort_matches(data->matches, match_count);
	finalize_matches(data->matches, match_count, pattern);
	return (data->matches);
}
