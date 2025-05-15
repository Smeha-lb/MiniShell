/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Check if character is a special token character
 */
static int	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

/**
 * Skip whitespace characters
 */
static int	skip_whitespace(char *input, int i)
{
	while (input[i] && ft_isspace(input[i]))
		i++;
	return (i);
}

/**
 * Handle quotes (single and double)
 */
static int	handle_quotes(char *input, int i, char quote, char **word)
{
	int		start;
	char	*tmp;

	start = i;
	i++;
	while (input[i] && input[i] != quote)
		i++;
	if (!input[i])
	{
		print_error("syntax error", "unclosed quote");
		return (-1);
	}
	tmp = ft_substr(input, start, i - start + 1);
	if (!tmp)
		return (-1);
	*word = ft_strjoin(*word, tmp);
	free(tmp);
	if (!*word)
		return (-1);
	return (i + 1);
}

/**
 * Extract a word token
 */
static int	extract_word(char *input, int i, t_token **tokens)
{
	char	*word;
	char	quote;
	char	*tmp;
	char	*substr;

	word = ft_strdup("");
	if (!word)
		return (-1);
	while (input[i] && !ft_isspace(input[i]) && !is_special_char(input[i]))
	{
		if (input[i] == '\'' || input[i] == '"')
		{
			quote = input[i];
			i = handle_quotes(input, i, quote, &word);
			if (i == -1)
			{
				free(word);
				return (-1);
			}
		}
		else
		{
			substr = ft_substr(input, i, 1);
			if (!substr)
			{
				free(word);
				return (-1);
			}
			tmp = ft_strjoin(word, substr);
			free(word);
			free(substr);
			if (!tmp)
				return (-1);
			word = tmp;
			i++;
		}
	}
	add_token(tokens, TOKEN_WORD, word);
	return (i);
}

/**
 * Extract a special token (pipe, redirection)
 */
static int	extract_special(char *input, int i, t_token **tokens)
{
	if (input[i] == '|')
	{
		add_token(tokens, TOKEN_PIPE, ft_strdup("|"));
		return (i + 1);
	}
	else if (input[i] == '<')
	{
		if (input[i + 1] && input[i + 1] == '<')
		{
			add_token(tokens, TOKEN_HEREDOC, ft_strdup("<<"));
			return (i + 2);
		}
		add_token(tokens, TOKEN_REDIR_IN, ft_strdup("<"));
		return (i + 1);
	}
	else if (input[i] == '>')
	{
		if (input[i + 1] && input[i + 1] == '>')
		{
			add_token(tokens, TOKEN_REDIR_APPEND, ft_strdup(">>"));
			return (i + 2);
		}
		add_token(tokens, TOKEN_REDIR_OUT, ft_strdup(">"));
		return (i + 1);
	}
	return (i + 1);
}

/**
 * Tokenize the input string
 */
t_token	*tokenize(char *input)
{
	t_token	*tokens;
	int		i;

	tokens = NULL;
	i = 0;
	while (input[i])
	{
		i = skip_whitespace(input, i);
		if (!input[i])
			break ;
		if (is_special_char(input[i]))
			i = extract_special(input, i, &tokens);
		else
			i = extract_word(input, i, &tokens);
		if (i == -1)
		{
			free_tokens(tokens);
			return (NULL);
		}
	}
	add_token(&tokens, TOKEN_END, ft_strdup(""));
	return (tokens);
} 