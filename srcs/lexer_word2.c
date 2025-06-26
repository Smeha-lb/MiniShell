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
	char	*expanded;

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
	expanded = expand_variables(shell, word, 0);
	free(word);
	add_expanded_tokens(shell, expanded);
	free(expanded);
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

int	process_word_loop(char *input, int *i, char *word, t_shell *shell)
{
	int				j;
	t_temp_var_data	data;

	j = 0;
	while (input[*i] && !is_word_delimiter(input[*i], input[*i + 1]))
	{
		if (input[*i] == '$' && input[*i + 1] == '\"')
			(*i)++;
		data.input = input;
		data.i = i;
		data.j = &j;
		data.word = &word;
		if (process_word_character(data, shell))
			return (-1);
	}
	word[j] = '\0';
	return (j);
}

int	handle_complex_word(char *input, int *i, t_shell *shell, int quoted)
{
	int				j;
	char			*word;
	int				buffer_size;

	buffer_size = calculate_word_buffer_size(input, *i);
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	j = process_word_loop(input, i, word, shell);
	if (j < 0 || j >= buffer_size)
	{
		free(word);
		return (1);
	}
	add_to_token_list(&shell->tokens, create_token(word, TOKEN_WORD, quoted));
	free(word);
	return (0);
}
