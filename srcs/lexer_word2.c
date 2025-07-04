#include "../includes/minishell.h"

int	is_word_delimiter(char c, char next_c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>'
		|| c == '(' || c == ')' || (c == '&' && next_c == '&'));
}

int	extract_word(char *input, int *i, t_shell *shell)
{
	int		start;
	int		j;
	char	*word;

	start = *i;
	while (input[*i] && !is_word_delimiter(input[*i], input[*i + 1]))
		(*i)++;
	word = (char *)malloc((*i - start + 1) * sizeof(char));
	if (!word)
		return (1);
	j = 0;
	while (start < *i)
		word[j++] = input[start++];
	word[j] = '\0';
	add_to_token_list(&shell->tokens, create_token(word, TOKEN_WORD, 0));
	free(word);
	return (0);
}

int	handle_word(char *input, int *i, t_shell *shell)
{
	int				start_pos;
	int				result;

	start_pos = *i;
	if (input[*i] == '$' && (ft_isalnum(input[*i + 1])
			|| input[*i + 1] == '_' || input[*i + 1] == '?'))
	{
		result = handle_simple_variable(shell, input, i);
		if (result >= 0)
			return (result);
		*i = start_pos;
	}
	return (handle_complex_word(input, i, shell, 0));
}

int	process_word_loop(char *input, int *i, char *word, t_shell *shell, int *quote_type)
{
	int				j;
	int				result;
	t_temp_var_data	data;

	j = 0;
	*quote_type = 0;
	while (input[*i] && !is_word_delimiter(input[*i], input[*i + 1]))
	{
		if (input[*i] == '$' && input[*i + 1] == '\"')
			(*i)++;
		data.input = input;
		data.i = i;
		data.j = &j;
		data.word = &word;
		result = process_word_character(data, shell);
		if (result < 0)
			return (-1);
		if (result > 0 && *quote_type == 0)
			*quote_type = result;
	}
	word[j] = '\0';
	return (j);
}

int	handle_complex_word(char *input, int *i, t_shell *shell, int quoted)
{
	char			*word;
	int				buffer_size;
	int				quote_type;
	t_word_data		word_data;

	buffer_size = calculate_word_buffer_size(input, *i);
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	prepare_word_data(input, i, &word_data);
	word_data.j = process_word_loop(input, i, word, shell, &quote_type);
	if (word_data.j < 0 || word_data.j >= buffer_size)
	{
		free(word);
		return (1);
	}
	if (quote_type > 0)
		quoted = quote_type;
	add_to_token_list(&shell->tokens, create_token(word, TOKEN_WORD, quoted));
	free(word);
	return (0);
}
