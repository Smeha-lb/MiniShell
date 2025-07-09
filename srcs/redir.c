/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:12:20 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:12:21 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	add_redir(t_redir **head, int type, char *file, int quoted)
{
	t_redir	*new_redir;
	t_redir	*current;

	new_redir = (t_redir *)malloc(sizeof(t_redir));
	if (!new_redir)
		return ;
	new_redir->type = type;
	new_redir->file = ft_strdup(file);
	new_redir->quoted = quoted;
	new_redir->next = NULL;
	if (!*head)
	{
		*head = new_redir;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

t_token	*handle_redir(t_token *token, t_command *cmd)
{
	t_token_type	type;
	t_token			*next_token;

	type = token->type;
	next_token = token->next;
	if (!next_token || next_token->type != TOKEN_WORD)
	{
		ft_putendl_fd("Error: Syntax error near redirection", 2);
		return (NULL);
	}
	if (type == TOKEN_REDIR_IN)
		add_redir(&cmd->redirs, TOKEN_REDIR_IN, next_token->value,
			next_token->quoted);
	else if (type == TOKEN_REDIR_OUT)
		add_redir(&cmd->redirs, TOKEN_REDIR_OUT, next_token->value,
			next_token->quoted);
	else if (type == TOKEN_REDIR_APPEND)
		add_redir(&cmd->redirs, TOKEN_REDIR_APPEND, next_token->value,
			next_token->quoted);
	else if (type == TOKEN_HEREDOC)
		add_redir(&cmd->redirs, TOKEN_HEREDOC, next_token->value,
			next_token->quoted);
	return (next_token);
}
