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

/**
 * Check if a filename matches a wildcard pattern
 */
int does_pattern_match(const char *pattern, const char *filename)
{
    // Skip matching hidden files unless explicitly specified
    if (filename[0] == '.' && pattern[0] != '.')
        return (0);
    
    // Base cases
    if (*pattern == '\0' && *filename == '\0')
        return (1);
    
    // If we hit a '*' in the pattern
    if (*pattern == '*')
    {
        // Skip consecutive '*' characters
        while (*(pattern + 1) == '*')
            pattern++;
        
        // '*' at the end of the pattern matches anything
        if (*(pattern + 1) == '\0')
            return (1);
        
        // Try to match the rest of the pattern with different positions in the filename
        while (*filename)
        {
            if (does_pattern_match(pattern + 1, filename))
                return (1);
            filename++;
        }
        
        // No match found for the rest of the pattern
        return (does_pattern_match(pattern + 1, filename));
    }
    
    // Handle character classes [...]
    if (*pattern == '[')
    {
        const char *closing_bracket = ft_strchr(pattern, ']');
        if (!closing_bracket)
            return (*pattern == *filename && does_pattern_match(pattern + 1, filename + 1));
        
        if (!is_in_char_class(*filename, pattern))
            return (0);
        
        return (does_pattern_match(closing_bracket + 1, filename + 1));
    }
    
    // Handle '?' wildcard (match any single character)
    if (*pattern == '?' && *filename != '\0')
        return (does_pattern_match(pattern + 1, filename + 1));
    
    // Regular character matching
    if (*pattern == *filename)
        return (does_pattern_match(pattern + 1, filename + 1));
    
    return (0);
}

/**
 * Sort the matched filenames alphabetically
 */
void sort_matches(char **matches, int count)
{
    int i, j;
    char *temp;
    
    for (i = 0; i < count - 1; i++)
    {
        for (j = i + 1; j < count; j++)
        {
            if (ft_strcmp(matches[i], matches[j]) > 0)
            {
                temp = matches[i];
                matches[i] = matches[j];
                matches[j] = temp;
            }
        }
    }
}

/**
 * Free an array of matched filenames
 */
void free_matches(char **matches)
{
    int i = 0;
    
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
 * Expand a wildcard pattern to matching filenames
 */
char **expand_wildcards(const char *pattern)
{
    DIR *dir;
    struct dirent *entry;
    char **matches = NULL;
    int match_count = 0;
    int capacity = 10; // Initial capacity
    
    // If the pattern doesn't contain wildcards, return it as is
    if (!has_wildcards(pattern))
    {
        matches = (char **)malloc(2 * sizeof(char *));
        matches[0] = ft_strdup(pattern);
        matches[1] = NULL;
        return (matches);
    }
    
    // Allocate initial array for matches
    matches = (char **)malloc(capacity * sizeof(char *));
    if (!matches)
        return (NULL);
    
    // Open the current directory
    dir = opendir(".");
    if (!dir)
    {
        matches[0] = NULL;
        return (matches);
    }
    
    // Read directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        if (does_pattern_match(pattern, entry->d_name))
        {
            // Resize if needed
            if (match_count >= capacity - 1)
            {
                capacity *= 2;
                char **new_matches = (char **)realloc(matches, capacity * sizeof(char *));
                if (!new_matches)
                {
                    free_matches(matches);
                    closedir(dir);
                    return (NULL);
                }
                matches = new_matches;
            }
            
            matches[match_count++] = ft_strdup(entry->d_name);
        }
    }
    
    // NULL-terminate the array
    matches[match_count] = NULL;
    
    // Close directory
    closedir(dir);
    
    // Sort matches alphabetically
    sort_matches(matches, match_count);
    
    // If no matches were found, return the original pattern
    if (match_count == 0)
    {
        matches[0] = ft_strdup(pattern);
        matches[1] = NULL;
    }
    
    return (matches);
}

/**
 * Join the expanded wildcards into a single space-separated string
 */
char *join_expanded_wildcards(char **matches)
{
    int i;
    int total_len = 0;
    char *result;
    int pos = 0;
    
    if (!matches || !matches[0])
        return (ft_strdup(""));
    
    // Calculate total length needed
    for (i = 0; matches[i]; i++)
    {
        total_len += ft_strlen(matches[i]);
        if (matches[i + 1]) // Add space if not the last word
            total_len++;
    }
    
    // Allocate memory for the result
    result = (char *)malloc((total_len + 1) * sizeof(char));
    if (!result)
        return (NULL);
    
    // Join the matches with spaces
    for (i = 0; matches[i]; i++)
    {
        ft_strlcpy(result + pos, matches[i], ft_strlen(matches[i]) + 1);
        pos += ft_strlen(matches[i]);
        
        if (matches[i + 1]) // Add space if not the last word
            result[pos++] = ' ';
    }
    
    result[total_len] = '\0';
    return (result);
} 