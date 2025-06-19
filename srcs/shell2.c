#include "../includes/minishell.h"

void	clean_tokens(t_shell *shell)
{
	shell->exit_status = 1;
	free_tokens(shell->tokens);
	shell->tokens = NULL;
}
