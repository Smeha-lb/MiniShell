#include "../includes/minishell.h"

int	check_nested_minishell(t_command *start_cmd)
{
	t_command	*cmd;
	int			is_nested;

	is_nested = 0;
	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		if (cmd->args && cmd->args[0]
			&& (ft_strcmp(cmd->args[0], "./minishell") == 0
				|| ft_strcmp(cmd->args[0], "minishell") == 0))
			is_nested = 1;
		cmd = cmd->next;
	}
	return (is_nested);
}

int	count_pipeline_cmds(t_command *start_cmd)
{
	t_command	*cmd;
	int			count;

	count = 0;
	cmd = start_cmd;
	while (cmd && (cmd == start_cmd || cmd->next_op == 0))
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}

void	execute_child_cmd(t_shell *shell, t_command *cmd)
{
	char	*path;

	if (is_builtin(cmd->args[0]))
		exit(execute_builtin(shell, cmd));
	path = find_command_path(shell, cmd->args[0]);
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

int	handle_pipeline_child(t_pipeline_child *pc)
{
	setup_child_pipes(pc->pipes, pc->i, pc->cmd_count);
	if (pc->cmd->redirs && setup_redirections(pc->shell, pc->cmd) != 0)
		exit(1);
	execute_child_cmd(pc->shell, pc->cmd);
	return (0);
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
