#include "../includes/minishell.h"

bool	handle_wildcards_token(t_token *token, t_command *cmd)
{
	parse_tokens_word(token->value, cmd, token->quoted);
	return (true);
}
