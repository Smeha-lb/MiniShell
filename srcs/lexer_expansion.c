#include "../includes/minishell.h"

int	handle_simple_variable_part2(t_shell *shell, char *input, int *i,
		int var_name_len)
{
	char	*var_ref;
	int		start;

	start = *i - 1;
	var_ref = ft_substr(input, start, var_name_len + 1);
	if (!var_ref)
		return (1);
	add_to_token_list(&shell->tokens, create_token(var_ref, TOKEN_WORD, 0));
	free(var_ref);
	*i += var_name_len;
	return (0);
}

// First check if we're at the end of the string
// If not at the end, safely check the next character
// Check if we're at the end of the string or if the next
// character is a delimiter
int	handle_simple_variable(t_shell *shell, char *input, int *i)
{
	int		var_name_len;
	int		temp_i;
	char	next_char;

	(*i)++;
	var_name_len = get_lexer_var_name_len(input + *i);
	temp_i = *i + var_name_len;
	if (input[temp_i] == '\0')
		return (handle_simple_variable_part2(shell, input, i, var_name_len));
	next_char = '\0';
	if (temp_i + 1 < (int)ft_strlen(input))
		next_char = input[temp_i + 1];
	if (is_var_delimiter(input[temp_i], next_char))
		return (handle_simple_variable_part2(shell, input, i, var_name_len));
	return (-1);
}

int	process_var_expansion(t_var_data *data, char **word, int *j)
{
	int	i;

	(*word)[(*j)++] = '$';
	i = 0;
	while (i < data->var_name_len)
	{
		(*word)[(*j)++] = data->var_name[i++];
	}
	free(data->var_name);
	free(data->var_value);
	return (0);
}

int	process_variable_expansion_part2(t_temp_var_data input_data,
		t_shell *shell, int var_name_len)
{
	t_var_data	data;

	data.var_name = ft_substr(input_data.input, *input_data.i, var_name_len);
	if (!data.var_name)
	{
		free(input_data.word);
		return (1);
	}
	data.var_value = get_variable_value(shell, data.var_name);
	if (!data.var_value)
	{
		free(data.var_name);
		free(input_data.word);
		return (1);
	}
	data.value_len = ft_strlen(data.var_value);
	data.var_name_len = var_name_len;
	*input_data.i += var_name_len;
	if (process_var_expansion(&data, input_data.word, input_data.j))
		return (1);
	return (0);
}

int	process_variable_expansion(t_temp_var_data input_data,
		t_shell *shell)
{
	int					var_name_len;

	(*input_data.i)++;
	var_name_len = get_lexer_var_name_len(input_data.input + *input_data.i);
	return (process_variable_expansion_part2(input_data, shell, var_name_len));
}
