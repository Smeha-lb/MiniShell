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

int	handle_subshell(t_shell *shell, t_command *cmd, int *exit_status)
{
	int	stdin_backup;
	int	stdout_backup;

	stdin_backup = dup(STDIN_FILENO);
	stdout_backup = dup(STDOUT_FILENO);
	if (cmd->redirs && setup_redirections(shell, cmd) != 0)
	{
		dup2(stdin_backup, STDIN_FILENO);
		dup2(stdout_backup, STDOUT_FILENO);
		close(stdin_backup);
		close(stdout_backup);
		*exit_status = 1;
		return (1);
	}
	*exit_status = execute_subshell(shell, cmd->subshell);
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	return (1);
}

int	setup_builtin_redirects(t_shell *shell, t_command *cmd, int *in, int *out)
{
	*in = dup(STDIN_FILENO);
	*out = dup(STDOUT_FILENO);
	if (setup_redirections(shell, cmd) != 0)
		return (1);
	return (0);
}
