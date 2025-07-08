#include "../includes/minishell.h"

/**
 * Update quote state for a character
 */
void	update_quote_state(char c, int prev_escaped,
	int *in_single_quotes, int *in_double_quotes)
{
	if (c == '\'' && !*in_double_quotes && !prev_escaped)
		*in_single_quotes = !*in_single_quotes;
	else if (c == '\"' && !*in_single_quotes && !prev_escaped)
		*in_double_quotes = !*in_double_quotes;
}

/**
 * Check if character at previous position is escape character
 */
int	is_escaped(const char *str, int i)
{
	return (i > 0 && str[i - 1] == '\\');
}

int	is_inside_single_quotes(const char *str, int pos)
{
	int	i;
	int	in_single_quotes;
	int	in_double_quotes;

	if (!str)
		return (0);
	i = 0;
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (i < pos && str[i])
	{
		update_quote_state(str[i], is_escaped(str, i),
			&in_single_quotes, &in_double_quotes);
		i++;
	}
	return (in_single_quotes);
}

int	is_inside_double_quotes(const char *str, int pos)
{
	int	i;
	int	in_double_quotes;
	int	in_single_quotes;

	if (!str)
		return (0);
	i = 0;
	in_double_quotes = 0;
	in_single_quotes = 0;
	while (i < pos && str[i])
	{
		update_quote_state(str[i], is_escaped(str, i),
			&in_single_quotes, &in_double_quotes);
		i++;
	}
	return (in_double_quotes);
}

int	get_var_name_len(const char *str)
{
	int	i;

	i = 0;
	if (str[0] == '?')
		return (1);
	while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
		i++;
	return (i);
}

char	*get_var_value(t_shell *shell, const char *var_name, int name_len)
{
	char	*name;
	char	*value;

	if (name_len == 1 && var_name[0] == '?')
		return (ft_itoa(shell->exit_status));
	name = ft_substr(var_name, 0, name_len);
	if (!name)
		return (NULL);
	value = get_env_value(shell, name);
	free(name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

/**
 * Calculate size contribution of a variable expansion
 */
int	calculate_var_size(t_shell *shell, const char *str, int *i)
{
	int		var_name_len;
	char	*var_value;
	int		size;

	(*i)++;
	var_name_len = get_var_name_len(str + *i);
	var_value = get_var_value(shell, str + *i, var_name_len);
	size = 0;
	if (var_value)
	{
		size = ft_strlen(var_value);
		free(var_value);
	}
	*i += var_name_len;
	return (size);
}

/**
 * Check if character at position needs variable expansion
 */
int	needs_var_expansion(const char *str, int i)
{
	return (str[i] == '$' && !is_inside_single_quotes(str, i)
		&& str[i + 1] && (ft_isalnum(str[i + 1])
			|| str[i + 1] == '_' || str[i + 1] == '?'));
}

int	calculate_expanded_size(t_shell *shell, const char *str)
{
	int		i;
	int		size;

	if (!str)
		return (0);
	i = 0;
	size = 0;
	while (str[i])
	{
		if (needs_var_expansion(str, i))
		{
			size += calculate_var_size(shell, str, &i);
		}
		else
		{
			size++;
			i++;
		}
	}
	return (size);
}

/**
 * Copy variable value to expanded string
 */
void	copy_var_to_expanded(t_shell *shell, const char *str,
	int *i, char *expanded, int *j)
{
	int		var_name_len;
	char	*var_value;

	(*i)++;
	var_name_len = get_var_name_len(str + *i);
	var_value = get_var_value(shell, str + *i, var_name_len);
	if (var_value)
	{
		ft_strlcpy(expanded + *j, var_value, ft_strlen(var_value) + 1);
		*j += ft_strlen(var_value);
		free(var_value);
	}
	*i += var_name_len;
}

char	*expand_variables_core(t_shell *shell, const char *str)
{
	int		i;
	int		j;
	char	*expanded;

	if (!str)
		return (NULL);
	expanded = (char *)malloc((calculate_expanded_size
				(shell, str) + 1) * sizeof(char));
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (needs_var_expansion(str, i))
		{
			copy_var_to_expanded(shell, str, &i, expanded, &j);
		}
		else
			expanded[j++] = str[i++];
	}
	expanded[j] = '\0';
	return (expanded);
}

char	*expand_token(t_shell *shell, const char *token)
{
	if (!token || !*token)
		return (NULL);
	return (expand_variables_core(shell, token));
}
