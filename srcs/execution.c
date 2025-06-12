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
	
	if (setup_redirections(shell, cmd) != 0)
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
		if (errno == ENOEXEC)
			print_error(cmd->args[0], NULL, "Permission denied");
		else
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
	int		status;

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
	
	// Wait for this specific child process to complete
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (0);
}

int	execute_single_command(t_shell *shell, t_command *cmd)
{
	int		result;

	if (!cmd->args)
		return (0);
	
	if (is_builtin(cmd->args[0]))
	{
		// Setup redirections for builtins
		int stdin_backup = dup(STDIN_FILENO);
		int stdout_backup = dup(STDOUT_FILENO);

		if (setup_redirections(shell, cmd) != 0)
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
		return execute_external_command(shell, cmd);
	}
}

// Execute a pipeline of commands (connected by pipes, not && or ||)
int execute_pipeline(t_shell *shell, t_command *start_cmd)
{
	t_command *cmd;
	pid_t *pids;
	int i, cmd_count;
	int exit_status = 0;
	int is_nested_minishell = 0;
	
	// Count commands in pipeline
	cmd_count = 0;
	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		// Check if any command in the pipeline is a minishell
		if (cmd->args && cmd->args[0] && 
			(ft_strcmp(cmd->args[0], "./minishell") == 0 ||
			 ft_strcmp(cmd->args[0], "minishell") == 0))
		{
			is_nested_minishell = 1;
		}
		cmd_count++;
		cmd = cmd->next;
	}
	
	// Allocate pid array
	pids = (pid_t *)malloc(sizeof(pid_t) * cmd_count);
	if (!pids)
	{
		print_error("malloc", NULL, strerror(errno));
		return (1);
	}
	
	// Setup pipes
	int **pipes = (int **)malloc(sizeof(int *) * (cmd_count - 1));
	if (!pipes)
	{
		free(pids);
		print_error("malloc", NULL, strerror(errno));
		return (1);
	}
	
	for (i = 0; i < cmd_count - 1; i++)
	{
		pipes[i] = (int *)malloc(sizeof(int) * 2);
		if (!pipes[i])
		{
			while (--i >= 0)
				free(pipes[i]);
			free(pipes);
			free(pids);
			print_error("malloc", NULL, strerror(errno));
			return (1);
		}
		if (pipe(pipes[i]) == -1)
		{
			while (i >= 0)
			{
				free(pipes[i]);
				i--;
			}
			free(pipes);
			free(pids);
			print_error("pipe", NULL, strerror(errno));
			return (1);
		}
	}
	
	// Execute commands in pipeline
	cmd = start_cmd;
	for (i = 0; i < cmd_count; i++)
	{
		pids[i] = fork();
		if (pids[i] == -1)
		{
			print_error("fork", NULL, strerror(errno));
			// Clean up pipes and continue
			continue;
		}
		else if (pids[i] == 0) // Child process
		{
			// Setup input from previous pipe if not first command
			if (i > 0)
			{
				dup2(pipes[i-1][0], STDIN_FILENO);
			}
			
			// Setup output to next pipe if not last command
			if (i < cmd_count - 1)
			{
				dup2(pipes[i][1], STDOUT_FILENO);
			}
			
			// Close all pipes in child
			for (int j = 0; j < cmd_count - 1; j++)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			
			// Apply any redirections for this command
			if (cmd->redirs && setup_redirections(shell, cmd) != 0)
				exit(1);
			
			// Execute command
			if (is_builtin(cmd->args[0]))
			{
				exit(execute_builtin(shell, cmd));
			}
			else
			{
				char *path = find_command_path(shell, cmd->args[0]);
				if (!path)
				{
					print_error(cmd->args[0], NULL, "command not found");
					exit(127);
				}
				if (execve(path, cmd->args, shell->env) == -1)
				{
					if (errno == ENOEXEC)
						print_error(cmd->args[0], NULL, "Permission denied");
					else
						print_error(cmd->args[0], NULL, strerror(errno));
					free(path);
					exit(127);
				}
			}
		}
		
		cmd = cmd->next;
	}
	
	// Parent process: close all pipes
	for (i = 0; i < cmd_count - 1; i++)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		free(pipes[i]);
	}
	free(pipes);
	
	// Wait for all child processes
	int status;
	
	// If running a nested minishell, ignore signals in the parent
	if (is_nested_minishell)
		ignore_signals();
	
	for (i = 0; i < cmd_count; i++)
	{
		waitpid(pids[i], &status, 0);
		if (i == cmd_count - 1) // Use the last command's exit status
		{
			if (WIFEXITED(status))
				exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_status = 128 + WTERMSIG(status);
		}
	}
	
	// Restore signal handling after all children are done
	if (is_nested_minishell)
		restore_signals();
	
	free(pids);
	return (exit_status);
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
	int			exit_status = 0;
	int			stdin_backup, stdout_backup;
	int			is_nested_minishell;
	
	cmd = shell->commands;
	while (cmd)
	{
		// Handle subshell execution if this is a subshell command
		if (cmd->is_subshell && cmd->subshell)
		{
			// Save stdin and stdout for restoration after command
			stdin_backup = dup(STDIN_FILENO);
			stdout_backup = dup(STDOUT_FILENO);
			
			// Setup redirections for the subshell
			if (cmd->redirs && setup_redirections(shell, cmd) != 0)
			{
				// Restore stdin and stdout if redirection fails
				dup2(stdin_backup, STDIN_FILENO);
				dup2(stdout_backup, STDOUT_FILENO);
				close(stdin_backup);
				close(stdout_backup);
				exit_status = 1;
				goto next_command;
			}
			
			// Execute the subshell
			exit_status = execute_subshell(shell, cmd->subshell);
			
			// Restore stdin and stdout
			dup2(stdin_backup, STDIN_FILENO);
			dup2(stdout_backup, STDOUT_FILENO);
			close(stdin_backup);
			close(stdout_backup);
			
			goto next_command;
		}
		
		// Each command gets its own redirection context
		stdin_backup = dup(STDIN_FILENO);
		stdout_backup = dup(STDOUT_FILENO);
		
		// Apply redirections only for the current command
		if (cmd->redirs)
		{
			if (setup_redirections(shell, cmd) != 0)
			{
				// If redirection fails, skip this command
				dup2(stdin_backup, STDIN_FILENO);
				dup2(stdout_backup, STDOUT_FILENO);
				close(stdin_backup);
				close(stdout_backup);
				
				// Check if the failure was due to heredoc interruption (Ctrl+C)
				if (g_signal_code == 130)
				{
					exit_status = 130;
					// Reset signal_code for next command
					g_signal_code = 0;
				}
				else
				{
					exit_status = 1;
				}
				
				goto next_command;
			}
		}
		
		// Check if this is a nested minishell command
		is_nested_minishell = (cmd->args && cmd->args[0] && 
			(ft_strcmp(cmd->args[0], "./minishell") == 0 ||
			 ft_strcmp(cmd->args[0], "minishell") == 0));
		
		// Execute the command with proper pipe handling
		if (cmd->next && cmd->next_op == 0) // Command is part of a pipeline
		{
			// Handle pipeline separately
			t_command *pipeline_start = cmd;
			
			// Find end of pipeline
			while (cmd->next && cmd->next_op == 0)
				cmd = cmd->next;
			
			// Execute the pipeline
			exit_status = execute_pipeline(shell, pipeline_start);
		}
		else // Single command (not part of a pipeline)
		{
			if (!cmd->args)
				exit_status = 0;
			else if (is_parent_builtin(cmd->args[0]))
			{
				exit_status = execute_builtin(shell, cmd);
			}
			else
			{
				pid_t pid = fork();
				if (pid == 0) // Child
				{
					char *path = find_command_path(shell, cmd->args[0]);
					if (!path)
					{
						print_error(cmd->args[0], NULL, "command not found");
						exit(127);
					}
					if (execve(path, cmd->args, shell->env) == -1)
					{
						print_error(cmd->args[0], NULL, strerror(errno));
						free(path);
						exit(127);
					}
				}
				else if (pid > 0) // Parent
				{
					// If we're running a nested minishell, ignore signals in the parent
					if (is_nested_minishell)
						ignore_signals();
						
					int status;
					waitpid(pid, &status, 0);
					
					// Restore signal handling after child is done
					if (is_nested_minishell)
						restore_signals();
						
					if (WIFEXITED(status))
						exit_status = WEXITSTATUS(status);
					else if (WIFSIGNALED(status))
						exit_status = 128 + WTERMSIG(status);
				}
				else // Fork failed
				{
					print_error("fork", NULL, strerror(errno));
					exit_status = 1;
				}
			}
		}
		
		// Restore stdin and stdout for each command
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		
next_command:
		// Check logical operators for next command
		if (cmd->next_op == 1 && exit_status == 0) // AND operator with success
			cmd = cmd->next;
		else if (cmd->next_op == 1 && exit_status != 0) // AND operator with failure
		{
			// Skip to next command after the following OR, if any
			cmd = cmd->next;
			while (cmd && cmd->next && cmd->next_op == 1)
				cmd = cmd->next;
		}
		else if (cmd->next_op == 2 && exit_status != 0) // OR operator with failure
			cmd = cmd->next;
		else if (cmd->next_op == 2 && exit_status == 0) // OR operator with success
		{
			// Skip to next command after the following AND, if any
			cmd = cmd->next;
			while (cmd && cmd->next && cmd->next_op == 2)
				cmd = cmd->next;
		}
		else
			cmd = cmd->next;
	}
	
	return (exit_status);
}



// Function to execute a subshell command
int execute_subshell(t_shell *shell, t_command *subshell_cmd)
{
    pid_t pid;
    int status;
    int exit_status = 0;
    
    // Subshells always execute in a forked process
    pid = fork();
    
    if (pid == -1)
    {
        print_error("fork", NULL, strerror(errno));
        return 1;
    }
    else if (pid == 0) // Child process
    {
        // Setup a new signal handling for the subshell
        setup_signals();
        
        // Execute the subshell commands
        t_shell subshell;
        
        // Copy shell properties to ensure proper environment for variable expansion
        subshell.env = copy_env(shell->env);
        subshell.exit_status = shell->exit_status;
        subshell.running = 1;
        subshell.tokens = NULL;
        subshell.commands = subshell_cmd;
        
        // Execute commands in the subshell context
        exit_status = execute_commands(&subshell);
        
        // Clean up
        free_array(subshell.env);
        
        // Exit with the subshell's exit status
        exit(exit_status);
    }
    else // Parent process
    {
        // Wait for the subshell to complete
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status))
            exit_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            exit_status = 128 + WTERMSIG(status);
    }
    
    return exit_status;
} 