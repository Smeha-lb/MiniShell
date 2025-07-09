/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_args2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:11:35 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:11:35 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	handle_wildcards_token(t_token *token, t_command *cmd)
{
	parse_tokens_word(token->value, cmd, token->quoted);
	return (true);
}
