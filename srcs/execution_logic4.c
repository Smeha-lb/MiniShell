#include "../includes/minishell.h"

int	execute_current_command(t_shell *shell, t_command **cmd, int *exit_status)
{
	if (!(*cmd)->args)
	{
		*exit_status = 0;
		return (0);
	}
	split_command_if_needed(shell, *cmd);
	if (!(*cmd)->args || !(*cmd)->args[0])
	{
		*exit_status = 0;
		return (0);
	}
	if (is_builtin((*cmd)->args[0]))
		*exit_status = execute_builtin(shell, *cmd);
	else
		*exit_status = execute_external_command(shell, *cmd);
	return (0);
} 