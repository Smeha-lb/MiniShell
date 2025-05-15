/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Create a new pipeline
 */
static t_pipeline	*create_pipeline(void)
{
	t_pipeline	*pipeline;

	pipeline = (t_pipeline *)malloc(sizeof(t_pipeline));
	if (!pipeline)
		return (NULL);
	pipeline->commands = NULL;
	pipeline->next = NULL;
	return (pipeline);
}

/**
 * Add a command to a pipeline
 */
static void	add_command(t_pipeline *pipeline, t_command *cmd)
{
	t_command	*current;

	if (!cmd)
		return ;
	if (!pipeline->commands)
	{
		pipeline->commands = cmd;
		return ;
	}
	current = pipeline->commands;
	while (current->next)
		current = current->next;
	current->next = cmd;
}

/**
 * Parse a pipeline from tokens
 */
static t_pipeline	*parse_pipeline(t_token **tokens)
{
	t_pipeline	*pipeline;
	t_command	*cmd;

	pipeline = create_pipeline();
	if (!pipeline)
		return (NULL);
	while (*tokens && (*tokens)->type != TOKEN_END)
	{
		cmd = parse_command(tokens);
		if (!cmd)
		{
			free_ast(pipeline);
			return (NULL);
		}
		add_command(pipeline, cmd);
		if ((*tokens)->type == TOKEN_PIPE)
			*tokens = (*tokens)->next;
		else
			break ;
	}
	return (pipeline);
}

/**
 * Parse tokens into an AST
 */
t_pipeline	*parse(t_token *tokens)
{
	t_pipeline	*ast;
	t_token		*current;

	if (!tokens)
		return (NULL);
	current = tokens;
	ast = parse_pipeline(&current);
	if (!ast)
		return (NULL);
	return (ast);
} 