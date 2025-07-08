#include "../includes/minishell.h"

/**
 * Update quote status for the last token based on its value
 */
void	update_last_token_quotes(t_shell *shell)
{
	t_token	*last_token;
	char	*token_value;
	int		token_len;

	if (!shell->tokens)
		return;
	last_token = shell->tokens;
	while (last_token->next)
		last_token = last_token->next;
	token_value = last_token->value;
	token_len = ft_strlen(token_value);
	if (token_len >= 2)
	{
		if (token_value[0] == '\'' && token_value[token_len - 1] == '\'')
			last_token->quoted = 1;
		else if (token_value[0] == '\"'
			&& token_value[token_len - 1] == '\"')
			last_token->quoted = 2;
	}
}

static bool	process_token_at_position(t_shell *shell, char *input, int *i)
{
	if (input[*i] == ' ' || input[*i] == '\t')
	{
		(*i)++;
		return (true);
	}
	if (handle_special_char(shell, input, i))
		return (true);
	if (handle_word(input, i, shell))
		return (false);
	update_last_token_quotes(shell);
	return (true);
}

bool	tokenize_input(t_shell *shell, char *input)
{
	int	i;

	i = 0;
	shell->tokens = NULL;
	while (input[i])
	{
		if (!process_token_at_position(shell, input, &i))
		{
			return (false);
		}
	}
	return (true);
}
