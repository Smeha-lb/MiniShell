#include "../includes/minishell.h"

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
	char	*result;

	print_error("DEBUG", "expand_token input", (char *)token);
	if (!token || !*token)
		return (NULL);
	
	result = expand_variables_core(shell, token);
	print_error("DEBUG", "expand_token result", result ? result : "NULL");
	
	return (result);
} 