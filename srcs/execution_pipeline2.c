#include "../includes/minishell.h"

int	handle_pipeline_helper(t_pipeline_child *pc,
		t_shell subshell)
{
	subshell.running = 1;
	subshell.tokens = NULL;
	subshell.commands = pc->cmd->subshell;
	return (execute_commands(&subshell));
}

int	handle_pipeline_child(t_pipeline_child *pc)
{
	int		exit_status;
	t_shell	subshell;

	// Set up pipes for this child process
	// This redirects stdin/stdout as needed
	setup_child_pipes(pc->pipes, pc->i, pc->cmd_count);
	
	// Handle subshell execution if needed
	if (pc->cmd->is_subshell && pc->cmd->subshell)
	{
		subshell.env = copy_env(pc->shell->env);
		subshell.exit_status = pc->shell->exit_status;
		exit_status = handle_pipeline_helper(pc, subshell);
		free_array(subshell.env);
		exit(exit_status);
	}
	
	// Handle redirections if needed
	if (pc->cmd->redirs && setup_redirections(pc->shell, pc->cmd) != 0)
	{
		if (pc->shell->tokens)
			free_tokens(pc->shell->tokens);
		if (pc->shell->commands)
			free_commands(pc->shell->commands);
		exit(1);
	}
	
	// Execute the command - this will not return
	execute_child_cmd(pc->shell, pc->cmd);
	return (0);  // Never reached
}
