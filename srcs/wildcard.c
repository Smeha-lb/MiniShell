#include "../includes/minishell.h"

/**
 * Check if a string contains wildcard characters
 */
int has_wildcards(const char *str)
{
	if (!str)
		return (0);
	
	while (*str)
	{
		if (*str == '*' || *str == '?' || *str == '[')
			return (1);
		str++;
	}
	return (0);
}

/**
 * Check if a character is in a character class [...]
 */
static int is_in_char_class(char c, const char *class_str)
{
	// Skip the opening bracket
	class_str++;
	
	// Check for negation
	int negate = 0;
	if (*class_str == '!')
	{
		negate = 1;
		class_str++;
	}
	
	// Check for range (e.g., [a-z])
	while (*class_str && *class_str != ']')
	{
		if (class_str[0] != '-' && class_str[1] == '-' && class_str[2] != ']')
		{
			// We have a range
			if (c >= class_str[0] && c <= class_str[2])
				return (negate ? 0 : 1);
			
			class_str += 3;
		}
		else
		{
			// Regular character comparison
			if (c == *class_str)
				return (negate ? 0 : 1);
			class_str++;
		}
	}
	
	return (negate ? 1 : 0);
}

int does_pattern_match(const char *pattern, const char *filename)
{
	if (filename[0] == '.' && pattern[0] != '.')
		return (0);
	if (*pattern == '\0' && *filename == '\0')
		return (1);
	if (*pattern == '*')
	{
		while (*(pattern + 1) == '*')
			pattern++;
		if (*(pattern + 1) == '\0')
			return (1);
		while (*filename)
		{
			if (does_pattern_match(pattern + 1, filename))
				return (1);
			filename++;
		}	
		return (does_pattern_match(pattern + 1, filename));
	}
	if (*pattern == '[')
	{
		const char *closing_bracket = ft_strchr(pattern, ']');
		if (!closing_bracket)
			return (*pattern == *filename && does_pattern_match(pattern + 1, filename + 1));
		
		if (!is_in_char_class(*filename, pattern))
			return (0);
		
		return (does_pattern_match(closing_bracket + 1, filename + 1));
	}
	if (*pattern == '?' && *filename != '\0')
		return (does_pattern_match(pattern + 1, filename + 1));
	if (*pattern == *filename)
		return (does_pattern_match(pattern + 1, filename + 1));
	return (0);
}

void sort_matches(char **matches, int count)
{
	int		i;
	int		j;
	char	*temp;
	
	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(matches[i], matches[j]) > 0)
			{
				temp = matches[i];
				matches[i] = matches[j];
				matches[j] = temp;
			}
			j++;
		}
		i++;
	}
}

void free_matches(char **matches)
{
	int i;

	i = 0;
	if (!matches)
		return;
	while (matches[i])
	{
		free(matches[i]);
		i++;
	}
	free(matches);
}

/**
 * Split a path into directory and filename parts
 */
static void split_path(const char *path, char **dir_part, char **file_part)
{
	char *last_slash = ft_strchr(path, '/');
	
	if (!last_slash)
	{
		// No directory part
		*dir_part = ft_strdup(".");
		*file_part = ft_strdup(path);
	}
	else
	{
		// Split into directory and filename parts
		int dir_len = last_slash - path + 1;
		*dir_part = ft_substr(path, 0, dir_len);
		*file_part = ft_strdup(last_slash + 1);
	}
}

/**
 * Join directory and filename parts
 */
static char *join_path(const char *dir, const char *file)
{
	char *result;
	int dir_len = ft_strlen(dir);
	
	// Handle special case for current directory
	if (ft_strcmp(dir, ".") == 0 && dir_len == 1)
		return ft_strdup(file);
	
	// Check if dir already ends with a slash
	if (dir[dir_len - 1] == '/')
		result = ft_strjoin(dir, file);
	else
	{
		// Need to add a slash between dir and file
		result = ft_strjoin(dir, "/");
		result = ft_strjoin_free(result, file);
	}
	
	return result;
}

char **expand_wildcards(const char *pattern)
{
	DIR *dir;
	struct dirent *entry;
	char **matches = NULL;
	int match_count = 0;
	int capacity = 10;
	char *dir_part;
	char *file_part;
	
	// If no wildcards, return the pattern as is
	if (!has_wildcards(pattern))
	{
		matches = (char **)malloc(2 * sizeof(char *));
		if (!matches)
			return NULL;
		matches[0] = ft_strdup(pattern);
		matches[1] = NULL;
		return (matches);
	}
	
	// Split the pattern into directory and file parts
	split_path(pattern, &dir_part, &file_part);
	
	matches = (char **)malloc(capacity * sizeof(char *));
	if (!matches)
	{
		free(dir_part);
		free(file_part);
		return (NULL);
	}
	
	// Open the directory
	dir = opendir(dir_part);
	if (!dir)
	{
		free(dir_part);
		free(file_part);
		matches[0] = NULL;
		return (matches);
	}
	
	// Read directory entries and match against the pattern
	while ((entry = readdir(dir)) != NULL)
	{
		if (does_pattern_match(file_part, entry->d_name))
		{
			if (match_count >= capacity - 1)
			{
				capacity *= 2;
				char **new_matches = (char **)realloc(matches, capacity * sizeof(char *));
				if (!new_matches)
				{
					free_matches(matches);
					free(dir_part);
					free(file_part);
					closedir(dir);
					return (NULL);
				}
				matches = new_matches;
			}
			// Join directory and matched filename
			matches[match_count++] = join_path(dir_part, entry->d_name);
		}
	}
	
	matches[match_count] = NULL;
	closedir(dir);
	free(dir_part);
	free(file_part);
	
	// Sort the matches
	sort_matches(matches, match_count);
	
	// If no matches, return the original pattern
	if (match_count == 0)
	{
		matches[0] = ft_strdup(pattern);
		matches[1] = NULL;
	}
	
	return (matches);
}

char *join_expanded_wildcards(char **matches)
{
	int i;
	int total_len = 0;
	char *result;
	int pos = 0;
	
	if (!matches || !matches[0])
		return (ft_strdup(""));
	
	i = 0;
	while (matches[i])
	{
		total_len += ft_strlen(matches[i]);
		if (matches[i + 1])
			total_len++;
		i++;
	}
	
	result = (char *)malloc((total_len + 1) * sizeof(char));
	if (!result)
		return (NULL);
	
	i = 0;
	while (matches[i])
	{
		ft_strlcpy(result + pos, matches[i], ft_strlen(matches[i]) + 1);
		pos += ft_strlen(matches[i]);
		
		if (matches[i + 1])
			result[pos++] = ' ';
		i++;
	}
	
	result[total_len] = '\0';
	return (result);
}
