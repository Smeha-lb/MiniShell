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
		shell->exit_status = 0;
		return (0);
	}
	split_command_if_needed(shell, *cmd);
	if (!(*cmd)->args || !(*cmd)->args[0])
	{
		*exit_status = 0;
		shell->exit_status = 0;
		return (0);
	}
	if (is_builtin((*cmd)->args[0]))
		*exit_status = execute_builtin(shell, *cmd);
	else
		*exit_status = execute_external_command(shell, *cmd);
	shell->exit_status = *exit_status;
	return (0);
}

/**
 * Check if a single argument needs expansion
 */
int	arg_needs_expansion(t_command *cmd, int i)
{
	if (should_split_arg(cmd->args[i])
		&& (!cmd->arg_quoted || !cmd->arg_quoted[i]))
		return (1);
	if (cmd->arg_quoted && cmd->arg_quoted[i] == 1)
		return (0);
	if (cmd->arg_quoted && cmd->arg_quoted[i]
		&& ft_strchr(cmd->args[i], '$'))
		return (1);
	if (!cmd->arg_quoted && ft_strchr(cmd->args[i], '$'))
		return (1);
	return (0);
}
