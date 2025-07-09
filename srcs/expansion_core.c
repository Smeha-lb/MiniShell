#include "../includes/minishell.h"

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
