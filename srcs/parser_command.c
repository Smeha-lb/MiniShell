#include "../includes/minishell.h"

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->arg_quoted = NULL;
	cmd->redirs = NULL;
	cmd->pipe_in = -1;
	cmd->pipe_out = -1;
	cmd->next_op = 0;
	cmd->is_subshell = 0;
	cmd->subshell = NULL;
	cmd->next = NULL;
	return (cmd);
}

bool	handle_pipe_token(t_token *token, t_command **cmd_ptr,
		t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_PIPE);
		return (false);
	}
	if (!cmd->args && !cmd->is_subshell && !cmd->redirs)
	{
		parse_tokens_err(cmd_head, ERR_PIPE);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}

bool	handle_and_token(t_token *token, t_command **cmd_ptr,
		t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_AND
		|| token->next->type == TOKEN_OR || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_AND);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next_op = 1;
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}

bool	handle_or_token(t_token *token, t_command **cmd_ptr,
		t_command *cmd_head)
{
	t_command	*cmd;
	t_command	*new_cmd;

	cmd = *cmd_ptr;
	if (!token->next || token->next->type == TOKEN_AND
		|| token->next->type == TOKEN_OR || token->next->type == TOKEN_PIPE)
	{
		parse_tokens_err(cmd_head, ERR_OR);
		return (false);
	}
	new_cmd = create_command();
	if (!new_cmd)
	{
		parse_tokens_err(cmd_head, "Memory allocation failed");
		return (false);
	}
	cmd->next_op = 2;
	cmd->next = new_cmd;
	*cmd_ptr = new_cmd;
	return (true);
}
