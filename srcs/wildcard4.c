/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:14:19 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:14:20 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	sort_matches(char **matches, int count)
{
	int		i;
	int		j;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(matches[i], matches[j]) > 0)
				swap_matches(matches, i, j);
			j++;
		}
		i++;
	}
}

	// Always use current directory, ignore any directory in the path
void	split_path(const char *path, char **dir_part, char **file_part)
{
	*dir_part = ft_strdup(".");
	*file_part = ft_strdup(path);
}

char	*join_path(const char *dir, const char *file)
{
	char	*result;
	int		dir_len;

	dir_len = ft_strlen(dir);
	if (ft_strcmp(dir, ".") == 0 && dir_len == 1)
		return (ft_strdup(file));
	if (dir[dir_len - 1] == '/')
		result = ft_strjoin(dir, file);
	else
	{
		result = ft_strjoin(dir, "/");
		result = ft_strjoin_free(result, file);
	}
	return (result);
}

char	**handle_no_wildcards(const char *pattern)
{
	char	**matches;

	matches = (char **)malloc(2 * sizeof(char *));
	if (!matches)
		return (NULL);
	matches[0] = ft_strdup(pattern);
	matches[1] = NULL;
	return (matches);
}
