#include "../includes/minishell.h"

bool	handle_word_token(t_token *token, t_command *cmd)
{
	if (cmd->is_subshell)
	{
		ft_putendl_fd(
			"Error: Syntax error: missing operator after subshell",
			2);
		return (false);
	}
	if (!token->quoted && has_wildcards(token->value))
		return (handle_wildcards_token(token, cmd));
	add_arg_with_quoted(cmd, token->value, token->quoted);
	return (true);
}

bool	handle_redirection_token(t_token **token_ptr, t_command *cmd,
								t_command *cmd_head)
{
	t_token	*result;

	result = handle_redir(*token_ptr, cmd);
	if (!result)
	{
		parse_tokens_err(cmd_head, "Redirection error");
		return (false);
	}
	*token_ptr = result;
	return (true);
}

bool	handle_parenthesis(t_token **token_ptr, t_command *cmd,
			t_command *cmd_head)
{
	t_token		*end_paren;
	t_command	*subshell_cmds;

	if (cmd->args)
	{
		parse_tokens_err(cmd_head,
			"Syntax error: missing operator before subshell");
		return (false);
	}
	end_paren = find_matching_paren(*token_ptr);
	if (!end_paren)
	{
		parse_tokens_err(cmd_head, ERR_RPAREN);
		return (false);
	}
	subshell_cmds = parse_subshell(*token_ptr, end_paren);
	if (!subshell_cmds)
	{
		parse_tokens_err(cmd_head, "Failed to parse subshell");
		return (false);
	}
	cmd->is_subshell = 1;
	cmd->subshell = subshell_cmds;
	*token_ptr = end_paren;
	return (true);
}
