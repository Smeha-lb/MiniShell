/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard5.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:14:22 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:14:23 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	**init_matches(int capacity)
{
	char	**matches;

	matches = (char **)malloc(capacity * sizeof(char *));
	if (!matches)
		return (NULL);
	return (matches);
}

char	**handle_dir_error(char *dir_part, char *file_part)
{
	char	**matches;

	matches = init_matches(1);
	if (!matches)
	{
		free(dir_part);
		free(file_part);
		return (NULL);
	}
	matches[0] = NULL;
	free(dir_part);
	free(file_part);
	return (matches);
}

static char	**resize_matches(t_match_data *data)
{
	char	**new_matches;

	data->capacity *= 2;
	new_matches = (char **)realloc(data->matches,
			data->capacity * sizeof(char *));
	if (!new_matches)
	{
		free_array(data->matches);
		free(data->dir_part);
		free(data->file_part);
		closedir(data->dir);
		return (NULL);
	}
	return (new_matches);
}

void	finalize_matches(char **matches, int match_count,
		const char *pattern)
{
	if (match_count == 0)
	{
		matches[0] = ft_strdup(pattern);
		matches[1] = NULL;
	}
}

int	add_matching_entry(t_match_data *data, char *entry_name)
{
	if (data->match_count >= data->capacity - 1)
	{
		data->matches = resize_matches(data);
		if (!data->matches)
			return (-1);
	}
	data->matches[(data->match_count)++] = join_path
		(data->dir_part, entry_name);
	return (0);
}
