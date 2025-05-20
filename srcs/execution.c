#include "../includes/minishell.h"

char	*find_command_path(t_shell *shell, char *cmd)
{
	char	**paths;
	char	*path;
	char	*path_env;
	int		i;

	if (!cmd || ft_strlen(cmd) == 0)
		return (NULL);
	if (cmd[0] == '/' || (cmd[0] == '.' && cmd[1] == '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(shell, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		path = ft_strjoin(paths[i], "/");
		path = ft_strjoin_free(path, cmd);
		if (access(path, X_OK) == 0)
		{
			free_array(paths);
			return (path);
		}
		free(path);
		i++;
	}
	free_array(paths);
	return (NULL);
}

void	handle_child_process(t_shell *shell, t_command *cmd, char *path)
{
	char	**env_array;
	
	if (setup_redirections(cmd) != 0)
		exit(1);
	
	if (cmd->pipe_in != -1)
	{
		dup2(cmd->pipe_in, STDIN_FILENO);
		close(cmd->pipe_in);
	}
	if (cmd->pipe_out != -1)
	{
		dup2(cmd->pipe_out, STDOUT_FILENO);
		close(cmd->pipe_out);
	}
	
	env_array = shell->env;
	if (execve(path, cmd->args, env_array) == -1)
	{
		print_error(cmd->args[0], NULL, strerror(errno));
		exit(127);
	}
}

int	create_pipes(t_command *commands)
{
	int			pipefd[2];
	t_command	*current;
	t_command	*next;

	current = commands;
	while (current && current->next && current->next_op == 0)
	{
		next = current->next;
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", NULL, strerror(errno));
			return (1);
		}
		current->pipe_out = pipefd[1];
		next->pipe_in = pipefd[0];
		current = next;
	}
	return (0);
}

void	close_pipes(t_command *commands)
{
	t_command	*current;

	current = commands;
	while (current)
	{
		if (current->pipe_in != -1)
			close(current->pipe_in);
		if (current->pipe_out != -1)
			close(current->pipe_out);
		current = current->next;
	}
}

int	wait_for_all(pid_t last_pid)
{
	int	status;
	int	exit_status;
	
	(void)last_pid; // Unused parameter

	exit_status = 0;
	while (waitpid(-1, &status, 0) > 0)
	{
		if (WIFEXITED(status))
			exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			exit_status = 128 + WTERMSIG(status);
	}
	return (exit_status);
}

int	execute_external_command(t_shell *shell, t_command *cmd)
{
	char	*path;
	pid_t	pid;

	path = find_command_path(shell, cmd->args[0]);
	if (!path)
	{
		print_error(cmd->args[0], NULL, "command not found");
		return (127);
	}
	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, strerror(errno));
		free(path);
		return (1);
	}
	else if (pid == 0)
	{
		handle_child_process(shell, cmd, path);
	}
	free(path);
	return (pid);
}

int	execute_single_command(t_shell *shell, t_command *cmd)
{
	pid_t	pid;
	int		result;

	if (!cmd->args)
		return (0);
	
	if (is_builtin(cmd->args[0]))
	{
		// Setup redirections for builtins
		int stdin_backup = dup(STDIN_FILENO);
		int stdout_backup = dup(STDOUT_FILENO);

		if (setup_redirections(cmd) != 0)
			return (1);

		result = execute_builtin(shell, cmd);

		// Restore original stdin and stdout
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);

		return (result);
	}
	else
	{
		pid = execute_external_command(shell, cmd);
		if (pid > 0)
			return (wait_for_all(pid));
		return (pid);
	}
}

// Execute a pipeline of commands (connected by pipes, not && or ||)
int execute_pipeline(t_shell *shell, t_command *start_cmd)
{
	t_command *cmd;
	pid_t pid;
	pid_t last_pid;
	
	// Create pipes for the pipeline
	if (create_pipes(start_cmd) != 0)
		return (1);
	
	// Execute each command in the pipeline
	cmd = start_cmd;
	last_pid = -1;
	
	while (cmd && cmd->next_op == 0)
	{
		if (!cmd->args)
		{
			cmd = cmd->next;
			continue;
		}
		
		if (is_builtin(cmd->args[0]) && !cmd->next)
		{
			close_pipes(start_cmd);
			return (execute_builtin(shell, cmd));
		}
		
		if (is_builtin(cmd->args[0]))
		{
			pid = fork();
			if (pid == 0)
			{
				if (setup_redirections(cmd) != 0)
					exit(1);
				
				if (cmd->pipe_in != -1)
				{
					dup2(cmd->pipe_in, STDIN_FILENO);
					close(cmd->pipe_in);
				}
				if (cmd->pipe_out != -1)
				{
					dup2(cmd->pipe_out, STDOUT_FILENO);
					close(cmd->pipe_out);
				}
				
				exit(execute_builtin(shell, cmd));
			}
		}
		else
		{
			pid = execute_external_command(shell, cmd);
		}
		
		last_pid = pid;
		cmd = cmd->next;
	}
	
	close_pipes(start_cmd);
	return (wait_for_all(last_pid));
}

// Check if a command requires to be executed in the parent process
int is_parent_builtin(char *cmd_name)
{
    return (ft_strcmp(cmd_name, "cd") == 0 || 
            ft_strcmp(cmd_name, "exit") == 0 || 
            ft_strcmp(cmd_name, "export") == 0 || 
            ft_strcmp(cmd_name, "unset") == 0);
}

int	execute_commands(t_shell *shell)
{
	t_command	*cmd;
	int			exit_status;
	t_command	*start_of_pipeline;
	
	cmd = shell->commands;
	if (!cmd)
		return (0);
	
	exit_status = 0;
	while (cmd)
	{
		start_of_pipeline = cmd;
		
		// Find the end of the current pipeline or logical operation
		while (cmd->next && cmd->next_op == 0)
			cmd = cmd->next;
		
		// Special case for builtins that need to be executed in parent process
		if (start_of_pipeline->args && is_parent_builtin(start_of_pipeline->args[0]) && 
			start_of_pipeline == cmd)
		{
			exit_status = execute_single_command(shell, start_of_pipeline);
		}
		// Execute the pipeline
		else if (start_of_pipeline == cmd && !cmd->next)
		{
			// Single command
			exit_status = execute_single_command(shell, cmd);
		}
		else
		{
			// Pipeline
			exit_status = execute_pipeline(shell, start_of_pipeline);
		}
		
		// If there are no more commands, we're done
		if (!cmd->next)
			break;
		
		// Handle logical operators
		if (cmd->next_op == 1) // AND (&&)
		{
			if (exit_status != 0)
			{
				// Skip the next command(s) until we find || or the end
				cmd = cmd->next;
				while (cmd && cmd->next_op == 1)
					cmd = cmd->next;
			}
		}
		else if (cmd->next_op == 2) // OR (||)
		{
			if (exit_status == 0)
			{
				// Skip the next command(s) until we find && or the end
				cmd = cmd->next;
				while (cmd && cmd->next_op == 2)
					cmd = cmd->next;
			}
		}
		
		cmd = cmd->next;
	}
	
	return (exit_status);
} 