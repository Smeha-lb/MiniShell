#include "../includes/minishell.h"

static bool	process_token_at_position(t_shell *shell, char *input, int *i)
{
	int		is_single_quoted;
	t_token	*last_token;

	if (input[*i] == ' ' || input[*i] == '\t')
	{
		(*i)++;
		return (true);
	}
	if (handle_special_char(shell, input, i))
		return (true);
	is_single_quoted = (input[*i] == '\'');
	if (handle_word(input, i, shell))
		return (false);
	if (is_single_quoted && shell->tokens)
	{
		last_token = shell->tokens;
		while (last_token->next)
			last_token = last_token->next;
		last_token->quoted = 1;
	}
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
			return (false);
	}
	return (true);
}
