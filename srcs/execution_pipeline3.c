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

	count = 1;  // Start with 1 for the first command
	cmd = start_cmd;
	while (cmd->next && cmd->next_op == 0)
	{
		count++;
		cmd = cmd->next;
	}
	return (count);
}
