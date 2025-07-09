#include "../includes/minishell.h"

/**
 * Copy variable value to expanded string
 */
void	copy_var_to_expanded(t_shell *shell, t_var_expansion_data *data)
{
	int		var_name_len;
	char	*var_value;

	(*data->i)++;
	var_name_len = get_var_name_len(data->str + *data->i);
	var_value = get_var_value(shell, data->str + *data->i, var_name_len);
	if (var_value)
	{
		ft_strlcpy(data->expanded + *data->j,
			var_value, ft_strlen(var_value) + 1);
		*data->j += ft_strlen(var_value);
		free(var_value);
	}
	*data->i += var_name_len;
}

/**
 * Initialize expansion data structure
 */
void	init_expansion_data(t_var_expansion_data *data)
{
	data->in_quotes = 0;
}

/**
 * Process expansion loop
 */
void	process_expansion_loop(t_shell *shell, t_var_expansion_data *data)
{
	while (data->str[*data->i])
	{
		if (needs_var_expansion(data->str, *data->i))
		{
			copy_var_to_expanded(shell, data);
		}
		else
			data->expanded[(*data->j)++] = data->str[(*data->i)++];
	}
}

char	*expand_variables_core(t_shell *shell, const char *str)
{
	int						i;
	int						j;
	char					*expanded;
	t_var_expansion_data	data;

	if (!str)
		return (NULL);
	expanded = (char *)malloc((calculate_expanded_size
				(shell, str) + 1) * sizeof(char));
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	data.str = str;
	data.i = &i;
	data.expanded = expanded;
	data.j = &j;
	init_expansion_data(&data);
	process_expansion_loop(shell, &data);
	expanded[j] = '\0';
	return (expanded);
}

char	*expand_token(t_shell *shell, const char *token)
{
	if (!token || !*token)
		return (NULL);
	return (expand_variables_core(shell, token));
}
