#include "../includes/minishell.h"

/**
 * Count number of arguments in the array
 */
int	count_args(char **args)
{
	int	i;

	if (!args)
		return (0);
	i = 0;
	while (args[i])
		i++;
	return (i);
}

/**
 * Handle expansion failure with cleanup
 */
char	**handle_expansion_error(char **new_args, int current_index)
{
	while (--current_index >= 0)
		free(new_args[current_index]);
	free(new_args);
	return (NULL);
}

/**
 * Expand a single argument safely
 */
char	*expand_single_arg(t_shell *shell, char *arg)
{
	char	*expanded;

	expanded = expand_token(shell, arg);
	if (!expanded)
	{
		expanded = ft_strdup("");
		if (!expanded)
			return (NULL);
	}
	return (expanded);
}

// Expand variables in command arguments just before execution
char	**expand_command_args(t_shell *shell, char **args)
{
	int		i;
	int		arg_count;
	char	**new_args;

	arg_count = count_args(args);
	if (arg_count == 0)
		return (NULL);
	new_args = (char **)malloc((arg_count + 1) * sizeof(char *));
	if (!new_args)
		return (NULL);
	i = 0;
	while (args[i])
	{
		new_args[i] = expand_single_arg(shell, args[i]);
		if (!new_args[i])
			return (handle_expansion_error(new_args, i));
		i++;
	}
	new_args[i] = NULL;
	return (new_args);
}

// Free the expanded arguments array
void	free_expanded_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	setup_child_redirections(t_command *cmd)
{
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
}

void	handle_child_process(t_shell *shell, t_command *cmd, char *path)
{
	setup_child_redirections(cmd);
	if (execve(path, cmd->args, shell->env) == -1)
	{
		if (errno == ENOEXEC)
			print_error(cmd->args[0], NULL, "Permission denied");
		else
			print_error(cmd->args[0], NULL, strerror(errno));
		exit(127);
	}
}

int	fork_external_command(t_shell *shell, t_command *cmd, char *path)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", NULL, strerror(errno));
		free(path);
		return (-1);
	}
	else if (pid == 0)
		handle_child_process(shell, cmd, path);
	free(path);
	return (pid);
}

int	wait_for_external_command(pid_t pid)
{
	int	status;

	ignore_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (0);
}

int	execute_external_command(t_shell *shell, t_command *cmd)
{
	char	*path;
	pid_t	pid;
	char	**temp_cmd_args;

	if (!(*cmd->arg_quoted == 2) && !cmd->args[1])
	{
		temp_cmd_args = ft_split(cmd->args[0], ' ');
		free_cmd_args(cmd->args);
		free(cmd->arg_quoted);
		cmd->args = temp_cmd_args;
		cmd->arg_quoted = NULL;
	}
	path = find_command_path(shell, cmd->args[0]);
	if (!path)
	{
		print_error(cmd->args[0], NULL, "command not found");
		return (127);
	}
	pid = fork_external_command(shell, cmd, path);
	if (pid == -1)
		return (1);
	return (wait_for_external_command(pid));
}
