#include "../includes/minishell.h"

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
		// Handle single quotes (but only if we're not in double quotes)
		if (str[i] == '\'' && !in_double_quotes
			&& (i == 0 || str[i - 1] != '\\'))
		{
			in_single_quotes = !in_single_quotes;
		}
		// Handle double quotes (but only if we're not in single quotes)
		else if (str[i] == '\"' && !in_single_quotes
			&& (i == 0 || str[i - 1] != '\\'))
		{
			in_double_quotes = !in_double_quotes;
		}
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
		if (str[i] == '\'' && !in_double_quotes
			&& (i == 0 || str[i - 1] != '\\'))
			in_single_quotes = !in_single_quotes;
		else if (str[i] == '\"' && !in_single_quotes
			&& (i == 0 || str[i - 1] != '\\'))
			in_double_quotes = !in_double_quotes;
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

int	calculate_expanded_size(t_shell *shell, const char *str)
{
	int		i;
	int		size;
	char	*var_value;
	int		var_name_len;

	if (!str)
		return (0);
	i = 0;
	size = 0;
	while (str[i])
	{
		if (str[i] == '$' && !is_inside_single_quotes(str, i)
			&& str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
		{
			i++;
			var_name_len = get_var_name_len(str + i);
			var_value = get_var_value(shell, str + i, var_name_len);
			if (var_value)
			{
				size += ft_strlen(var_value);
				free(var_value);
			}
			i += var_name_len;
		}
		else
		{
			size++;
			i++;
		}
	}
	return (size);
}

char	*expand_variables_core(t_shell *shell, const char *str)
{
	int		i;
	int		j;
	char	*expanded;
	int		var_name_len;
	char	*var_value;

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
		if (str[i] == '$' && !is_inside_single_quotes(str, i)
			&& str[i + 1] && (ft_isalnum(str[i + 1])
				|| str[i + 1] == '_' || str[i + 1] == '?'))
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
