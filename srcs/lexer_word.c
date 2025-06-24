#include "../includes/minishell.h"

int	calculate_word_buffer_size(char *input, int i)
{
	int		buffer_size;
	int		j;
	char	quote;
	int		quote_start;

	buffer_size = 0;
	j = i;
	while (input[j] && !is_word_delimiter(input[j], input[j + 1]))
	{
		if (input[j] == '\'' || input[j] == '\"')
		{
			quote = input[j++];
			quote_start = j;
			while (input[j] && input[j] != quote)
				j++;
			if (!input[j])
				break ;
			if (quote == '\"')
				buffer_size += (j - quote_start) * 2;
			else
				buffer_size += (j - quote_start);
			j++;
		}
		else if (input[j] == '$' && (ft_isalnum(input[j + 1])
				|| input[j + 1] == '_' || input[j + 1] == '?'))
		{
			j++;
			while (input[j] && (ft_isalnum(input[j]) || input[j] == '_'
					|| (j == quote_start && input[j] == '?')))
				j++;
			buffer_size += 128;
		}
		else
		{
			buffer_size++;
			j++;
		}
	}
	return (buffer_size + 1024);
}

int	process_word_character(char *input, int *i, char **word, int *j,
		t_shell *shell)
{
	if (input[*i] == '\'' || input[*i] == '\"')
	{
		if (handle_quotes(input, i, word, j, shell))
			return (1);
	}
	else if (input[*i] == '$' && (ft_isalnum(input[*i + 1])
			|| input[*i + 1] == '_' || input[*i + 1] == '?'))
	{
		if (process_variable_expansion(input, i, word, j, shell))
			return (1);
	}
	else
	{
		(*word)[(*j)++] = input[(*i)++];
	}
	return (0);
}

int	handle_complex_word(char *input, int *i, t_shell *shell)
{
	int		j;
	char	*word;
	int		buffer_size;

	buffer_size = calculate_word_buffer_size(input, *i);
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	j = 0;
	while (input[*i] && !is_word_delimiter(input[*i], input[*i + 1]))
	{
		if (process_word_character(input, i, &word, &j, shell))
			return (1);
		if (j >= buffer_size)
		{
			free(word);
			return (1);
		}
	}
	word[j] = '\0';
	add_to_token_list(&shell->tokens, create_token(word, TOKEN_WORD));
	free(word);
	return (0);
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
	int	start_pos;
	int	result;

	start_pos = *i;
	if (input[*i] == '$' && (ft_isalnum(input[*i + 1])
			|| input[*i + 1] == '_' || input[*i + 1] == '?'))
	{
		result = handle_simple_variable(shell, input, i);
		if (result >= 0)
			return (result);
		*i = start_pos;
	}
	return (handle_complex_word(input, i, shell));
}
