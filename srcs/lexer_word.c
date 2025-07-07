#include "../includes/minishell.h"

int	handle_quoted_section(t_buffer_calc *calc, char quote)
{
	calc->j++;
	calc->quote_start = calc->j;
	while (calc->input[calc->j] && calc->input[calc->j] != quote)
		calc->j++;
	if (!calc->input[calc->j])
		return (0);
	if (quote == '\"')
		calc->buffer_size += (calc->j - calc->quote_start) * 2;
	else
		calc->buffer_size += (calc->j - calc->quote_start);
	calc->j++;
	return (1);
}

int	handle_variable_section(t_buffer_calc *calc)
{
	calc->j++;
	while (calc->input[calc->j] && (ft_isalnum(calc->input[calc->j])
			|| calc->input[calc->j] == '_'
			|| (calc->j == calc->quote_start && calc->input[calc->j] == '?')))
		calc->j++;
	calc->buffer_size += 128;
	return (1);
}

int	handle_regular_char(t_buffer_calc *calc)
{
	calc->buffer_size++;
	calc->j++;
	return (1);
}

int	calculate_word_buffer_size(char *input, int i)
{
	t_buffer_calc	calc;

	calc.input = input;
	calc.buffer_size = 0;
	calc.j = i;
	calc.quote_start = 0;
	while (input[calc.j] && !is_word_delimiter(input[calc.j],
			input[calc.j + 1]))
	{
		if (input[calc.j] == '\'' || input[calc.j] == '\"')
		{
			if (!handle_quoted_section(&calc, input[calc.j]))
				break ;
		}
		else if (input[calc.j] == '$' && (ft_isalnum(input[calc.j + 1])
				|| input[calc.j + 1] == '_' || input[calc.j + 1] == '?'))
			handle_variable_section(&calc);
		else
			handle_regular_char(&calc);
	}
	return (calc.buffer_size + 1024);
}

// Skip the dollar sign if it's directly followed by double quotes
int	process_word_character(t_temp_var_data data, t_shell *shell)
{
	int	quote_result;

	if (data.input[*data.i] == '$' && data.input[*data.i + 1] == '\"')
		(*data.i)++;
	if (data.input[*data.i] == '\'' || data.input[*data.i] == '\"')
	{
		quote_result = handle_quotes(data, shell);
		if (quote_result < 0)
			return (-1);
		return (quote_result);
	}
	else if (data.input[*data.i] == '$' && (ft_isalnum(data.input[*data.i + 1])
			|| data.input[*data.i + 1] == '_'
			|| data.input[*data.i + 1] == '?')
		&& !is_inside_single_quotes(data.input, *data.i))
	{
		if (process_variable_expansion
			(data, shell))
			return (-1);
	}
	else
		(*data.word)[(*data.j)++] = data.input[(*data.i)++];
	return (0);
}
