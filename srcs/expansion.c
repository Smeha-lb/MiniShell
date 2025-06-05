#include "../includes/minishell.h"

// Get the length of the variable name
static int get_var_name_len(char *str)
{
    int i;

    i = 0;
    if (str[i] == '?')
        return (1);
    
    while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
        i++;
    return (i);
}

// Get the value of a variable, handle special case for $?
static char *get_var_value(t_shell *shell, char *var_name, int name_len)
{
    char *name;
    char *value;

    // Handle $? for exit status
    if (name_len == 1 && var_name[0] == '?')
    {
        char *exit_status = ft_itoa(shell->exit_status);
        if (!exit_status)
            return (NULL);
        return (exit_status);
    }

    // Extract the variable name
    name = ft_substr(var_name, 0, name_len);
    if (!name)
        return (NULL);

    // Get the value from environment
    value = get_env_value(shell, name);
    free(name);

    if (value)
        return (ft_strdup(value));
    return (ft_strdup(""));
}

// Calculate the length of the expanded string
static int calculate_expanded_length(t_shell *shell, char *str)
{
    int i;
    int length;
    int var_name_len;
    char *var_value;

    i = 0;
    length = 0;

    while (str[i])
    {
        if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1]) || str[i + 1] == '_' || str[i + 1] == '?'))
        {
            i++;
            var_name_len = get_var_name_len(str + i);
            var_value = get_var_value(shell, str + i, var_name_len);
            if (var_value)
            {
                length += ft_strlen(var_value);
                free(var_value);
            }
            i += var_name_len;
        }
        else
        {
            length++;
            i++;
        }
    }
    return (length);
}

// Expand variables in a string
char *expand_variables(t_shell *shell, char *str, int in_quotes)
{
    int i;
    int j;
    int var_name_len;
    char *expanded;
    char *var_value;

    (void)in_quotes; // Mark as intentionally unused

    if (!str)
        return (NULL);

    // Calculate the length of the expanded string
    expanded = (char *)malloc((calculate_expanded_length(shell, str) + 1) * sizeof(char));
    if (!expanded)
        return (NULL);

    i = 0;
    j = 0;

    while (str[i])
    {
        if (str[i] == '$' && str[i + 1] && (ft_isalnum(str[i + 1]) || str[i + 1] == '_' || str[i + 1] == '?'))
        {
            i++;
            var_name_len = get_var_name_len(str + i);
            var_value = get_var_value(shell, str + i, var_name_len);
            
            if (var_value)
            {
                ft_strlcpy(expanded + j, var_value, ft_strlen(var_value) + 1);
                j += ft_strlen(var_value);
                free(var_value);
            }
            
            i += var_name_len;
        }
        else
        {
            expanded[j++] = str[i++];
        }
    }
    
    expanded[j] = '\0';
    return (expanded);
}

// Expand wildcards in an unquoted string segment
static char *expand_segment_wildcards(char *str)
{
    char **matches;
    char *result;
    
    // Check if the segment contains any wildcards
    if (!has_wildcards(str))
        return (ft_strdup(str));
    
    // Expand wildcards
    matches = expand_wildcards(str);
    if (!matches)
        return (ft_strdup(str));
    
    // Join matches into a space-separated string
    result = join_expanded_wildcards(matches);
    
    // Free the matches
    free_matches(matches);
    
    return (result);
}

// Handle expansion for a string, respecting quote rules
char *handle_expansion(t_shell *shell, char *str)
{
    int i;
    int start;
    char quote;
    char *result;
    char *temp;
    char *expanded;
    char *wildcarded;

    if (!str)
        return (NULL);

    result = ft_strdup("");
    if (!result)
        return (NULL);

    i = 0;
    while (str[i])
    {
        // Handle quoted segments
        if (str[i] == '\'' || str[i] == '\"')
        {
            quote = str[i++];
            start = i;
            
            // Find the closing quote
            while (str[i] && str[i] != quote)
                i++;
            
            if (!str[i])  // Unclosed quote
            {
                free(result);
                return (ft_strdup(str));  // Return original string if quotes aren't closed
            }
            
            // Extract the content inside quotes
            temp = ft_substr(str, start, i - start);
            
            // Expand variables inside double quotes but not single quotes
            if (quote == '\"')
            {
                expanded = expand_variables(shell, temp, 1);
                free(temp);
                temp = expanded;
            }
            
            // Append to result
            expanded = ft_strjoin(result, temp);
            free(result);
            free(temp);
            result = expanded;
            
            i++; // Skip the closing quote
        }
        // Handle unquoted text
        else
        {
            start = i;
            // Find the next quote or end of string
            while (str[i] && str[i] != '\'' && str[i] != '\"')
                i++;
            
            // Extract and expand the unquoted text
            temp = ft_substr(str, start, i - start);
            
            // First expand variables
            expanded = expand_variables(shell, temp, 0);
            free(temp);
            
            // Then expand wildcards (only in unquoted text)
            wildcarded = expand_segment_wildcards(expanded);
            free(expanded);
            
            // Append to result
            temp = ft_strjoin(result, wildcarded);
            free(result);
            free(wildcarded);
            result = temp;
        }
    }
    
    return (result);
} 