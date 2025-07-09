/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:11:13 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:11:13 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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
	if (result > 0)
	{
		if (*quote_type == 0)
			*quote_type = result;
		else if (*quote_type != result)
			*quote_type = 3;
	}
	return (0);
}

int	process_word_loop(t_word_processing_data *wdata, int *quote_type)
{
	int				j;
	t_temp_var_data	data;

	j = 0;
	*quote_type = 0;
	while (wdata->input[*wdata->i] && !is_word_delimiter
		(wdata->input[*wdata->i], wdata->input[*wdata->i + 1]))
	{
		if (wdata->input[*wdata->i] == '$'
			&& wdata->input[*wdata->i + 1] == '\"')
			(*wdata->i)++;
		data.input = wdata->input;
		data.i = wdata->i;
		data.j = &j;
		data.word = &wdata->word;
		if (process_single_character(data, wdata->shell, quote_type) < 0)
			return (-1);
	}
	wdata->word[j] = '\0';
	return (j);
}

/**
* Process word with proper buffer validation
*/
int	process_word_with_validation(t_word_processing_data *wdata, int buffer_size)
{
	int				quote_type;
	t_word_data		word_data;

	prepare_word_data(wdata->input, wdata->i, &word_data);
	word_data.j = process_word_loop(wdata, &quote_type);
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
	char					*word;
	int						buffer_size;
	int						quote_type;
	t_word_processing_data	wdata;

	buffer_size = calculate_word_buffer_size(input, *i);
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	wdata.input = input;
	wdata.i = i;
	wdata.word = word;
	wdata.shell = shell;
	quote_type = process_word_with_validation(&wdata, buffer_size);
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
