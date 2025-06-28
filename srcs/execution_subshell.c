#include "../includes/minishell.h"

void	init_subshell(t_shell *subshell
		, t_shell *shell, t_command *subshell_cmd)
{
	subshell->env = copy_env(shell->env);
	subshell->exit_status = shell->exit_status;
	subshell->running = 1;
	subshell->tokens = NULL;
	subshell->commands = subshell_cmd;
}

int	handle_parent_process(pid_t pid, int *exit_status)
{
	int	status;

	ignore_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
		*exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		*exit_status = 128 + WTERMSIG(status);
	return (*exit_status);
}

int	execute_subshell(t_shell *shell, t_command *subshell_cmd)
{
	pid_t	pid;
	int		exit_status;
	t_shell	subshell;

	pid = fork();
	exit_status = 0;
	if (pid == -1)
	{
		print_error("fork", NULL, strerror(errno));
		return (1);
	}
	else if (pid == 0)
	{
		setup_signals();
		init_subshell(&subshell, shell, subshell_cmd);
		exit_status = execute_commands(&subshell);
		free_array(subshell.env);
		exit(exit_status);
	}
	else
		return (handle_parent_process(pid, &exit_status));
}

void	setup_pipeline_redirections(t_command *cmd)
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

int	handle_redirection_error(int stdin_backup, int stdout_backup,
									int *exit_status)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	*exit_status = 1;
	return (1);
}
