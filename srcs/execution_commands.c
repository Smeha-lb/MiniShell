#include "../includes/minishell.h"

int	execute_command_with_redirects(t_shell *shell, t_command **cmd,
		int *exit_status)
{
	int	stdin_backup;
	int	stdout_backup;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if ((*cmd)->redirs && setup_redirections(shell, *cmd) != 0)
		return (handle_redirection_failure(stdin_backup, stdout_backup,
				exit_status));
	execute_current_command(shell, cmd, exit_status);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (0);
}

int	execute_commands(t_shell *shell)
{
	t_command	*cmd;
	int			exit_status;

	cmd = shell->commands;
	exit_status = 0;
	while (cmd)
	{
		if (cmd->is_subshell && cmd->subshell && cmd->pipe_in == -1
			&& cmd->pipe_out == -1)
		{
			if (handle_subshell(shell, cmd, &exit_status))
			{
				cmd = cmd->next;
				continue ;
			}
		}
		if (execute_command_with_redirects(shell, &cmd, &exit_status))
			cmd = cmd->next;
		else
			handle_next_command(&cmd, exit_status);
	}
	return (exit_status);
}
