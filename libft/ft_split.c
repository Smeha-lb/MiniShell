#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;
	int	i;

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

static char	*get_word(char const *s, char c, int *i)
{
	char	*word;
	int		word_len;
	int		j;

	word_len = 0;
	while (s[*i + word_len] && s[*i + word_len] != c)
		word_len++;
	word = (char *)malloc((word_len + 1) * sizeof(char));
	if (!word)
		return (NULL);
	j = 0;
	while (j < word_len)
	{
		word[j] = s[*i];
		j++;
		(*i)++;
	}
	word[j] = '\0';
	return (word);
}

static void	free_split(char **split, int j)
{
	while (j >= 0)
	{
		free(split[j]);
		j--;
	}
	free(split);
}

char	**ft_split(char const *s, char c)
{
	char	**split;
	int		words;
	int		i;
	int		j;

	if (!s)
		return (NULL);
	words = count_words(s, c);
	split = (char **)malloc((words + 1) * sizeof(char *));
	if (!split)
		return (NULL);
	i = 0;
	j = 0;
	while (j < words)
	{
		while (s[i] && s[i] == c)
			i++;
		split[j] = get_word(s, c, &i);
		if (!split[j])
			return (free_split(split, j - 1), NULL);
		j++;
	}
	split[j] = NULL;
	return (split);
}
