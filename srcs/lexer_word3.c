#include "../includes/minishell.h"

int	prepare_word_data(char *input, int *i, t_word_data *data)
{
	data->end = *i;
	while (input[data->end] && !is_word_delimiter(input[data->end],
			input[data->end + 1]))
		data->end++;
	data->has_vars = 0;
	data->j = *i;
	while (data->j < data->end)
	{
		if (input[data->j] == '$')
		{
			data->has_vars = 1;
			break ;
		}
		data->j++;
	}
	return (0);
}
