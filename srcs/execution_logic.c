#include "../includes/minishell.h"

void	handle_and_operator(t_command **cmd, int exit_status)
{
	if (exit_status == 0)
		*cmd = (*cmd)->next;
	else
	{
		*cmd = (*cmd)->next;
		while (*cmd && (*cmd)->next && (*cmd)->next_op == 1)
			*cmd = (*cmd)->next;
		if (*cmd && (*cmd)->next && (*cmd)->next_op == 2)
			*cmd = (*cmd)->next;
		else
			*cmd = NULL;
	}
}

void	handle_or_operator(t_command **cmd, int exit_status)
{
	if (exit_status != 0)
		*cmd = (*cmd)->next;
	else
	{
		*cmd = (*cmd)->next;
		while (*cmd && (*cmd)->next && (*cmd)->next_op == 2)
			*cmd = (*cmd)->next;
		if (*cmd && (*cmd)->next && (*cmd)->next_op == 1)
			*cmd = (*cmd)->next;
		else
			*cmd = NULL;
	}
}

void	handle_next_command(t_command **cmd, int exit_status)
{
	if ((*cmd)->next_op == 1)
		handle_and_operator(cmd, exit_status);
	else if ((*cmd)->next_op == 2)
		handle_or_operator(cmd, exit_status);
	else
		*cmd = (*cmd)->next;
}

int	handle_redirection_failure(int stdin_backup, int stdout_backup,
		int *exit_status)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	if (g_signal_code == 130)
	{
		*exit_status = 130;
		g_signal_code = 0;
	}
	else
		*exit_status = 1;
	return (1);
}

int	execute_current_command(t_shell *shell, t_command **cmd, int *exit_status)
{
	// Pipelines are now handled directly in execute_commands
	if (!(*cmd)->args)
		*exit_status = 0;
	else if (is_builtin((*cmd)->args[0]))
		*exit_status = execute_builtin(shell, *cmd);
	else
		*exit_status = execute_external_command(shell, *cmd);
	
	return (0);
}
