/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Create a new redirection
 */
static t_redir	*create_redirection(t_redir_type type, char *file)
{
	t_redir	*redir;

	redir = (t_redir *)malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = file;
	redir->next = NULL;
	return (redir);
}

/**
 * Add a redirection to a command
 */
static void	add_redirection(t_redir **redirs, t_redir *new_redir)
{
	t_redir	*current;

	if (!new_redir)
		return ;
	if (!*redirs)
	{
		*redirs = new_redir;
		return ;
	}
	current = *redirs;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

/**
 * Parse a redirection token
 */
t_redir	*parse_redirection(t_token **tokens)
{
	t_token		*token;
	t_redir		*redir;
	t_redir_type	type;

	token = *tokens;
	if (token->type == TOKEN_REDIR_IN)
		type = REDIR_IN;
	else if (token->type == TOKEN_REDIR_OUT)
		type = REDIR_OUT;
	else if (token->type == TOKEN_REDIR_APPEND)
		type = REDIR_APPEND;
	else if (token->type == TOKEN_HEREDOC)
		type = REDIR_HEREDOC;
	else
		return (NULL);
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != TOKEN_WORD)
	{
		print_error("syntax error", "missing redirection target");
		return (NULL);
	}
	redir = create_redirection(type, ft_strdup((*tokens)->value));
	*tokens = (*tokens)->next;
	return (redir);
}

/**
 * Create a new command
 */
static t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

/**
 * Add an argument to a command
 */
static int	add_argument(t_command *cmd, char *arg)
{
	char	**new_args;
	int		i;

	i = 0;
	if (!cmd->args)
	{
		cmd->args = (char **)malloc(sizeof(char *) * 2);
		if (!cmd->args)
			return (0);
		cmd->args[0] = ft_strdup(arg);
		cmd->args[1] = NULL;
		return (1);
	}
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return (0);
	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
	return (1);
}

/**
 * Parse a command from tokens
 */
t_command	*parse_command(t_token **tokens)
{
	t_command	*cmd;
	t_redir		*redir;

	cmd = create_command();
	if (!cmd)
		return (NULL);
	while (*tokens && (*tokens)->type != TOKEN_PIPE && (*tokens)->type != TOKEN_END)
	{
		if ((*tokens)->type == TOKEN_WORD)
		{
			if (!add_argument(cmd, (*tokens)->value))
			{
				free(cmd);
				return (NULL);
			}
			*tokens = (*tokens)->next;
		}
		else if ((*tokens)->type >= TOKEN_REDIR_IN && (*tokens)->type <= TOKEN_HEREDOC)
		{
			redir = parse_redirection(tokens);
			if (!redir)
			{
				free(cmd);
				return (NULL);
			}
			add_redirection(&cmd->redirections, redir);
		}
		else
		{
			*tokens = (*tokens)->next;
		}
	}
	return (cmd);
}

/**
 * Free an AST
 */
void	free_ast(t_pipeline *ast)
{
	t_pipeline	*pipeline;
	t_pipeline	*next_pipeline;
	t_command	*cmd;
	t_command	*next_cmd;
	t_redir		*redir;
	t_redir		*next_redir;
	int			i;

	pipeline = ast;
	while (pipeline)
	{
		next_pipeline = pipeline->next;
		cmd = pipeline->commands;
		while (cmd)
		{
			next_cmd = cmd->next;
			if (cmd->args)
			{
				i = 0;
				while (cmd->args[i])
					free(cmd->args[i++]);
				free(cmd->args);
			}
			redir = cmd->redirections;
			while (redir)
			{
				next_redir = redir->next;
				free(redir->file);
				free(redir);
				redir = next_redir;
			}
			free(cmd);
			cmd = next_cmd;
		}
		free(pipeline);
		pipeline = next_pipeline;
	}
} 