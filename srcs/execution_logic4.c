#include "../includes/minishell.h"

/**
 * Execute the current command and handle its exit status
 * Returns 0 on success, 1 on error
 */
int	execute_current_command(t_shell *shell, t_command **cmd, int *exit_status)
{
	if (!cmd || !*cmd)
		return (1);
		
	if (!(*cmd)->args)
	{
		*exit_status = 0;
		return (0);
	}
	
	// Split command if it contains variables that need expansion
	split_command_if_needed(shell, *cmd);
	
	if (!(*cmd)->args || !(*cmd)->args[0])
	{
		*exit_status = 0;
		return (0);
	}
	
	// Execute builtin or external command
	if (is_builtin((*cmd)->args[0]))
		*exit_status = execute_builtin(shell, *cmd);
	else
		*exit_status = execute_external_command(shell, *cmd);
		
	return (0);
} 