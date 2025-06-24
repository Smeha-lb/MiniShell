#include "../includes/minishell.h"

int	handle_simple_variable_part2(t_shell *shell, char *input, int *i,
		int var_name_len)
{
	t_var_data	data;

	data.var_name = ft_substr(input, *i, var_name_len);
	if (!data.var_name)
		return (1);
	data.var_value = get_variable_value(shell, data.var_name);
	if (!data.var_value)
	{
		free(data.var_name);
		return (1);
	}
	*i += var_name_len;
	add_expanded_tokens(shell, data.var_value);
	free(data.var_name);
	free(data.var_value);
	return (0);
}

int	handle_simple_variable(t_shell *shell, char *input, int *i)
{
	int		var_name_len;
	int		temp_i;

	(*i)++;
	var_name_len = get_var_name_len(input + *i);
	temp_i = *i + var_name_len;
	if (is_var_delimiter(input[temp_i], input[temp_i + 1]))
	{
		return (handle_simple_variable_part2(shell, input, i, var_name_len));
	}
	return (-1);
}

int	process_var_expansion(t_var_data *data, char **word, int *j)
{
	ft_strlcpy(*word + *j, data->var_value, data->value_len + 1);
	*j += data->value_len;
	free(data->var_name);
	free(data->var_value);
	return (0);
}

int	process_variable_expansion_part2(char *input, int *i, char **word, int *j,
		t_shell *shell, int var_name_len)
{
	t_var_data	data;

	data.var_name = ft_substr(input, *i, var_name_len);
	if (!data.var_name)
	{
		free(*word);
		return (1);
	}
	data.var_value = get_variable_value(shell, data.var_name);
	if (!data.var_value)
	{
		free(data.var_name);
		free(*word);
		return (1);
	}
	data.value_len = ft_strlen(data.var_value);
	*i += var_name_len;
	if (process_var_expansion(&data, word, j))
		return (1);
	return (0);
}

int	process_variable_expansion(char *input, int *i, char **word, int *j,
		t_shell *shell)
{
	int		var_name_len;

	(*i)++;
	var_name_len = get_var_name_len(input + *i);
	return (process_variable_expansion_part2(input, i, word, j,
			shell, var_name_len));
}
