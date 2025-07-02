#include "../includes/minishell.h"

void	handle_and_operator(t_command **cmd, int exit_status)
{
	if (exit_status == 0)
		*cmd = (*cmd)->next;
	else
	{
		*cmd = (*cmd)->next;
		while (*cmd && (*cmd)->next && (*cmd)->next_op == 1)
			*cmd = (*cmd)->next;
		if (*cmd && (*cmd)->next && (*cmd)->next_op == 2)
			*cmd = (*cmd)->next;
		else
			*cmd = NULL;
	}
}

void	handle_or_operator(t_command **cmd, int exit_status)
{
	if (exit_status != 0)
		*cmd = (*cmd)->next;
	else
	{
		*cmd = (*cmd)->next;
		while (*cmd && (*cmd)->next && (*cmd)->next_op == 2)
			*cmd = (*cmd)->next;
		if (*cmd && (*cmd)->next && (*cmd)->next_op == 1)
			*cmd = (*cmd)->next;
		else
			*cmd = NULL;
	}
}

void	handle_next_command(t_command **cmd, int exit_status)
{
	if ((*cmd)->next_op == 1)
		handle_and_operator(cmd, exit_status);
	else if ((*cmd)->next_op == 2)
		handle_or_operator(cmd, exit_status);
	else
		*cmd = (*cmd)->next;
}

void	handle_pipeline_next_command(t_command **cmd, int exit_status)
{
	int	next_op;

	if (!(*cmd) || !(*cmd)->next)
	{
		*cmd = NULL;
		return ;
	}
	next_op = (*cmd)->next_op;
	*cmd = (*cmd)->next;
	if ((next_op == 1 && exit_status != 0)
		|| (next_op == 2 && exit_status == 0))
	{
		while (*cmd && ((next_op == 1 && (*cmd)->next_op == 1)
				|| (next_op == 2 && (*cmd)->next_op == 2)))
			*cmd = (*cmd)->next;
		if (*cmd && ((next_op == 1 && (*cmd)->next_op == 2)
				|| (next_op == 2 && (*cmd)->next_op == 1)))
			*cmd = (*cmd)->next;
		else
			*cmd = NULL;
	}
}

int	handle_redirection_failure(int stdin_backup, int stdout_backup,
		int *exit_status)
{
	dup2(stdin_backup, STDIN_FILENO);
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdin_backup);
	close(stdout_backup);
	if (g_signal_code == 130)
	{
		*exit_status = 130;
		g_signal_code = 0;
	}
	else
		*exit_status = 1;
	return (1);
}

// Strip only outermost quotes from a string, preserve inner quotes
char	*strip_quotes(char *str)
{
	char	*result;
	int		len;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	if (len < 2)
	{
		result = ft_strdup(str);
		free(str);
		return (result);
	}
	if ((str[0] == '\'' && str[len - 1] == '\'') ||
		(str[0] == '\"' && str[len - 1] == '\"'))
	{
		result = ft_substr(str, 1, len - 2);
		free(str);
		return (result);
	}
	result = ft_strdup(str);
	free(str);
	return (result);
}

// Check if a string should be split after expansion
int	should_split_arg(const char *arg)
{
	if (ft_strchr(arg, '$'))
	{
		if (arg[0] != '\'')
			return (1);
	}
	return (0);
}

// Split expanded command if needed
void	split_command_if_needed(t_shell *shell, t_command *cmd)
{
	char	*expanded;
	char	**split_args;
	int		i;
	int		j;
	int		k;
	char	**new_args;
	int		*new_quoted;
	int		should_split;

	if (!cmd->args || !cmd->args[0])
		return ;
	i = 0;
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc(sizeof(char *) * (i * 10 + 1));
	new_quoted = (int *)malloc(sizeof(int) * (i * 10 + 1));
	if (!new_args || !new_quoted)
	{
		free(new_args);
		free(new_quoted);
		return ;
	}
	i = 0;
	k = 0;
	while (cmd->args[i])
	{
		should_split = should_split_arg(cmd->args[i]);
		expanded = expand_token(shell, cmd->args[i]);
		if (!expanded)
		{
			new_args[k] = ft_strdup(cmd->args[i]);
			new_quoted[k] = (cmd->arg_quoted ? cmd->arg_quoted[i] : 0);
			k++;
		}
		else if (!should_split)
		{
			new_args[k] = expanded;
			new_quoted[k] = (cmd->arg_quoted ? cmd->arg_quoted[i] : 0);
			k++;
		}
		else
		{
			split_args = ft_split(expanded, ' ');
			if (split_args)
			{
				j = 0;
				while (split_args[j])
				{
					new_args[k] = split_args[j];
					new_quoted[k] = 0;
					k++;
					j++;
				}
				free(split_args);
			}
			free(expanded);
		}
		i++;
	}
	new_args[k] = NULL;
	free_cmd_args(cmd->args);
	free(cmd->arg_quoted);
	cmd->args = new_args;
	cmd->arg_quoted = new_quoted;
}

int	execute_current_command(t_shell *shell, t_command **cmd, int *exit_status)
{
	if (!(*cmd)->args)
	{
		*exit_status = 0;
		return (0);
	}
	split_command_if_needed(shell, *cmd);
	if (!(*cmd)->args || !(*cmd)->args[0])
	{
		*exit_status = 0;
		return (0);
	}
	if (is_builtin((*cmd)->args[0]))
		*exit_status = execute_builtin(shell, *cmd);
	else
		*exit_status = execute_external_command(shell, *cmd);
	return (0);
}
