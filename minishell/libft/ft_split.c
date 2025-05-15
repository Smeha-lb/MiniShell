/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	count_words(char const *s, char c)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i])
			count++;
		while (s[i] && s[i] != c)
			i++;
	}
	return (count);
}

static size_t	word_len(char const *s, char c, size_t i)
{
	size_t	len;

	len = 0;
	while (s[i] && s[i] != c)
	{
		len++;
		i++;
	}
	return (len);
}

static void	free_split(char **split, size_t index)
{
	while (index > 0)
	{
		index--;
		free(split[index]);
	}
	free(split);
}

static char	**fill_split(char const *s, char c, char **split, size_t words)
{
	size_t	i;
	size_t	j;
	size_t	w_len;

	i = 0;
	j = 0;
	while (j < words)
	{
		while (s[i] && s[i] == c)
			i++;
		w_len = word_len(s, c, i);
		split[j] = ft_substr(s, i, w_len);
		if (!split[j])
		{
			free_split(split, j);
			return (NULL);
		}
		i += w_len;
		j++;
	}
	split[j] = NULL;
	return (split);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	size_t	words;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	split = (char **)malloc(sizeof(char *) * (words + 1));
	if (!split)
		return (NULL);
	return (fill_split(s, c, split, words));
} 