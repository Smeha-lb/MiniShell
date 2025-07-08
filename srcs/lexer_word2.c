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

/**
 * Handle special dollar quote case
 */
void	handle_dollar_quote(char *input, int *i)
{
	if (input[*i] == '$' && input[*i + 1] == '\"')
		(*i)++;
}

/**
 * Process a single character in word loop
 */
int	process_single_character(t_temp_var_data data,
		t_shell *shell, int *quote_type)
{
	int	result;

	result = process_word_character(data, shell);
	if (result < 0)
		return (-1);
	if (result > 0 && *quote_type == 0)
		*quote_type = result;
	return (0);
}

int	process_word_loop(char *input, int *i, char *word,
		t_shell *shell, int *quote_type)
{
	int				j;
	t_temp_var_data	data;

	j = 0;
	*quote_type = 0;
	while (input[*i] && !is_word_delimiter(input[*i], input[*i + 1]))
	{
		handle_dollar_quote(input, i);
		data.input = input;
		data.i = i;
		data.j = &j;
		data.word = &word;
		if (process_single_character(data, shell, quote_type) < 0)
			return (-1);
	}
	word[j] = '\0';
	return (j);
}

/**
 * Process word with proper buffer validation
 */
int	process_word_with_validation(char *input, int *i, char *word,
	t_shell *shell, int buffer_size)
{
	int				quote_type;
	t_word_data		word_data;

	prepare_word_data(input, i, &word_data);
	word_data.j = process_word_loop(input, i, word, shell, &quote_type);
	if (word_data.j < 0)
		return (-1);
	if (word_data.j >= buffer_size)
		return (-1);
	return (quote_type);
}

/**
 * Create and add word token
 */
int	create_word_token(char *word, int quoted, t_shell *shell)
{
	add_to_token_list(&shell->tokens, create_token(word, TOKEN_WORD, quoted));
	return (0);
}

int	handle_complex_word(char *input, int *i, t_shell *shell, int quoted)
{
	char			*word;
	int				buffer_size;
	int				quote_type;

	buffer_size = calculate_word_buffer_size(input, *i);
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	quote_type = process_word_with_validation(input, i, word,
			shell, buffer_size);
	if (quote_type < 0)
	{
		free(word);
		return (1);
	}
	if (quote_type > 0)
		quoted = quote_type;
	create_word_token(word, quoted, shell);
	free(word);
	return (0);
}
