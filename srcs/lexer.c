#include "../includes/minishell.h"

bool	tokenize_input(t_shell *shell, char *input)
{
	int	i;

	i = 0;
	shell->tokens = NULL;
	while (input[i])
	{
		if (input[i] == ' ' || input[i] == '\t')
			i++;
		else if (handle_special_char(shell, input, &i))
			continue ;
		else if (handle_word(input, &i, shell))
			return (false);
	}
	return (true);
}
