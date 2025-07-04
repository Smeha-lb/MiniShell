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

	setup_child_pipes(pc->pipes, pc->i, pc->cmd_count);
	if (pc->cmd->is_subshell && pc->cmd->subshell)
	{
		subshell.env = copy_env(pc->shell->env);
		subshell.exit_status = pc->shell->exit_status;
		exit_status = handle_pipeline_helper(pc, subshell);
		free_array(subshell.env);
		exit(exit_status);
	}
	
	// Expand variables in command arguments before execution
	split_command_if_needed(pc->shell, pc->cmd);
	
	if (pc->cmd->redirs && setup_redirections(pc->shell, pc->cmd) != 0)
	{
		if (pc->shell->tokens)
			free_tokens(pc->shell->tokens);
		if (pc->shell->commands)
			free_commands(pc->shell->commands);
		exit(1);
	}
	execute_child_cmd(pc->shell, pc->cmd);
	return (0);
}
