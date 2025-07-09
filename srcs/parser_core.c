/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:12:04 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:12:05 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	parse_tokens_err(t_command *cmd_head, char *msg)
{
	ft_putstr_fd("Error: ", 2);
	ft_putendl_fd(msg, 2);
	free_commands(cmd_head);
}

bool	process_token(t_token **token_ptr, t_command **cmd_ptr,
		t_command *cmd_head)
{
	t_token		*token;
	t_command	*cmd;

	token = *token_ptr;
	cmd = *cmd_ptr;
	if (token->type == TOKEN_WORD)
		return (handle_word_token(token, cmd, cmd_head));
	else if (token->type == TOKEN_PIPE)
		return (handle_pipe_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_AND)
		return (handle_and_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_OR)
		return (handle_or_token(token, cmd_ptr, cmd_head));
	else if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT
		|| token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC)
		return (handle_redirection_token(token_ptr, cmd, cmd_head));
	else if (token->type == TOKEN_LPAREN)
		return (handle_parenthesis(token_ptr, cmd, cmd_head));
	else if (token->type == TOKEN_RPAREN)
	{
		parse_tokens_err(cmd_head, ERR_RPAREN);
		return (false);
	}
	return (true);
}

bool	parse_tokens(t_shell *shell)
{
	t_token		*token;
	t_command	*cmd_head;
	t_command	*cmd;

	if (!shell->tokens)
		return (true);
	cmd_head = create_command();
	if (!cmd_head)
		return (false);
	cmd = cmd_head;
	token = shell->tokens;
	while (token)
	{
		if (!process_token(&token, &cmd, cmd_head))
			return (false);
		token = token->next;
	}
	shell->commands = cmd_head;
	return (true);
}
