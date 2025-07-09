/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:11:05 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:11:06 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
