/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 16:22:06 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/25 14:30:06 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

bool	token_is_redir(t_token token)
{
	return (token == WORD || token == IN || token == OUT || token == APPEND \
		|| token == HEREDOC);
}

t_token	parse_token(char chr, char next)
{
	if (chr == '|' && next == '|')
		return (OR);
	if (chr == '&' && next == '&')
		return (AND);
	if (chr == '>' && next == '>')
		return (APPEND);
	if (chr == '<' && next == '<')
		return (HEREDOC);
	if (chr == '>')
		return (OUT);
	if (chr == '<')
		return (IN);
	if (chr == ')')
		return (LEFT_PAREN);
	if (chr == '(')
		return (RIGHT_PAREN);
	if (chr == '|')
		return (PIPE);
	if (!chr)
		return (END);
	return (WORD);
}

bool	token_is_redir(t_token token)
{
	return (token == NOT || token == IN || token == OUT || token == APPEND \
		|| token == HEREDOC);
}

t_token	parse_token(char chr, char next)
{
	if (chr == '|' && next == '|')
		return (OR);
	if (chr == '&' && next == '&')
		return (AND);
	if (chr == '>' && next == '>')
		return (APPEND);
	if (chr == '<' && next == '<')
		return (HEREDOC);
	if (chr == '>')
		return (OUT);
	if (chr == '<')
		return (IN);
	if (chr == ')')
		return (LEFT_PAREN);
	if (chr == '(')
		return (RIGHT_PAREN);
	if (chr == '|')
		return (PIPE);
	if (!chr)
		return (END);
	return (NOT);
}

int	get_token_precedence(t_token token)
{
	if (token == IN || token == OUT || token == APPEND || token == HEREDOC)
		return (4);
	if (token == PIPE)
		return (3);
	if (token == OR || token == AND)
		return (2);
	return (1);
}
