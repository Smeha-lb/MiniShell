#include "../includes/minishell.h"

/**
 * Handle AND operator (&&) logic
 * If previous command succeeded (exit_status == 0), continue to next command
 * Otherwise, skip until next OR operator or end
 */
void	handle_and_operator(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
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

/**
 * Handle OR operator (||) logic
 * If previous command failed (exit_status != 0), continue to next command
 * Otherwise, skip until next AND operator or end
 */
void	handle_or_operator(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
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

/**
 * Route to appropriate operator handler based on next_op value
 * 1 = AND operator (&&)
 * 2 = OR operator (||)
 * 0 = No operator, just move to next command
 */
void	handle_next_command(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
	if ((*cmd)->next_op == 1)
		handle_and_operator(cmd, exit_status);
	else if ((*cmd)->next_op == 2)
		handle_or_operator(cmd, exit_status);
	else
		*cmd = (*cmd)->next;
}
