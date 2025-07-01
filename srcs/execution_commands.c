#include "../includes/minishell.h"

int	execute_command_with_redirects(t_shell *shell, t_command **cmd,
		int *exit_status)
{
	int	stdin_backup;
	int	stdout_backup;

	// Save the original stdin/stdout
	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	
	// Set up redirections if needed
	if ((*cmd)->redirs && setup_redirections(shell, *cmd) != 0)
		return (handle_redirection_failure(stdin_backup, stdout_backup,
				exit_status));
	
	// Execute the command
	execute_current_command(shell, cmd, exit_status);
	
	// Restore the original stdin/stdout
	if (dup2(stdin_backup, STDIN_FILENO) == -1)
		perror("dup2 stdin restore");
	if (dup2(stdout_backup, STDOUT_FILENO) == -1)
		perror("dup2 stdout restore");
	
	// Clean up
	close(stdin_backup);
	close(stdout_backup);
	return (0);
}

int	execute_commands(t_shell *shell)
{
	t_command	*cmd;
	int			exit_status;
	t_command	*pipeline_start;

	cmd = shell->commands;
	exit_status = 0;
	while (cmd)
	{
		// Check if this is the start of a pipeline
		if (cmd->next && cmd->next_op == 0)
		{
			pipeline_start = cmd;
			// Find the end of the pipeline - stop only at the last pipe command
			// A command is part of the pipeline if the PREVIOUS command's next_op is 0 (pipe)
			t_command *last_pipe_cmd = cmd;
			while (cmd->next && cmd->next_op == 0)
			{
				cmd = cmd->next;
				last_pipe_cmd = cmd;
			}
			
			// Execute the pipeline as a single unit
			// This ensures all pipe redirections are set up correctly
			exit_status = execute_pipeline(shell, pipeline_start);
			
			// Use the last command in the pipeline to determine next steps
			cmd = last_pipe_cmd;
			handle_pipeline_next_command(&cmd, exit_status);
			continue;
		}
		
		// Handle subshells
		if (cmd->is_subshell && cmd->subshell && cmd->pipe_in == -1
			&& cmd->pipe_out == -1)
		{
			if (handle_subshell(shell, cmd, &exit_status))
			{
				cmd = cmd->next;
				continue;
			}
		}
		
		// Handle regular commands
		if (execute_command_with_redirects(shell, &cmd, &exit_status))
			cmd = cmd->next;
		else
			handle_next_command(&cmd, exit_status);
	}
	return (exit_status);
}
