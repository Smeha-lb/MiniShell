#include "../includes/minishell.h"

static void	execute_child_cmd_helper(char *path, t_shell *shell, t_command *cmd)
{
	print_error(cmd->args[0], NULL, "Error in execve");
	free(path);
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	free_array(shell->env);
}

static void	execute_builtin_in_child(t_shell *shell, t_command *cmd)
{
	t_shell	builtin_shell;
	int		exit_code;
	int		stdout_fd;

	// Save the current stdout file descriptor
	// This is already set up by setup_child_pipes, but we want to make sure
	// the builtin command uses it correctly
	stdout_fd = dup(STDOUT_FILENO);
	
	// Create a clean shell environment for the builtin
	builtin_shell.env = copy_env(shell->env);
	builtin_shell.exit_status = shell->exit_status;
	builtin_shell.tokens = NULL;
	builtin_shell.commands = NULL;
	builtin_shell.running = 1;
	builtin_shell.previous_cmd = NULL;
	
	// Execute the builtin command - stdout is already redirected by setup_child_pipes
	exit_code = execute_builtin(&builtin_shell, cmd);
	
	// Make sure any buffered output is flushed to the pipe
	fflush(stdout);
	
	// Clean up and exit
	close(stdout_fd);
	free_array(builtin_shell.env);
	exit(exit_code);
}

static void	handle_command_not_found(t_shell *shell, t_command *cmd)
{
	print_error(cmd->args[0], NULL, "command not found");
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	free_array(shell->env);
	exit(127);
}

void	execute_child_cmd(t_shell *shell, t_command *cmd)
{
	char	*path;

	if (!cmd->args || !cmd->args[0])
		exit(0);
	if (is_builtin(cmd->args[0]))
		execute_builtin_in_child(shell, cmd);
	path = find_command_path(shell, cmd->args[0]);
	if (!path)
		handle_command_not_found(shell, cmd);
	if (execve(path, cmd->args, shell->env) == -1)
	{
		execute_child_cmd_helper(path, shell, cmd);
		exit(127);
	}
}

int	wait_for_pipeline(pid_t *pids, int cmd_count, int is_nested)
{
	int	status;
	int	exit_status;
	int	i;

	if (is_nested)
		ignore_signals();
	i = 0;
	exit_status = 0;
	while (i < cmd_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1)
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	if (is_nested)
		setup_signals();
	return (exit_status);
}
