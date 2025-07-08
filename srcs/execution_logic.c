#include "../includes/minishell.h"

/**
 * Handle AND operator (&&) logic
 * If previous command succeeded (exit_status == 0), continue to next command
 * Otherwise, skip until next OR operator or end
 */
void	handle_and_operator(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
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

/**
 * Handle OR operator (||) logic
 * If previous command failed (exit_status != 0), continue to next command
 * Otherwise, skip until next AND operator or end
 */
void	handle_or_operator(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
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

/**
 * Route to appropriate operator handler based on next_op value
 * 1 = AND operator (&&)
 * 2 = OR operator (||)
 * 0 = No operator, just move to next command
 */
void	handle_next_command(t_command **cmd, int exit_status)
{
	if (!cmd || !*cmd)
		return ;
	if ((*cmd)->next_op == 1)
		handle_and_operator(cmd, exit_status);
	else if ((*cmd)->next_op == 2)
		handle_or_operator(cmd, exit_status);
	else
		*cmd = (*cmd)->next;
}

/**
 * Handle pipeline next command with operator logic
 */
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

/**
 * Handle redirection failure by restoring stdin/stdout and setting exit status
 */
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

/**
 * Strip outermost quotes from a string, preserving inner quotes
 */
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
	if ((str[0] == '\'' && str[len - 1] == '\'')
		|| (str[0] == '\"' && str[len - 1] == '\"'))
	{
		result = ft_substr(str, 1, len - 2);
		free(str);
		return (result);
	}
	result = ft_strdup(str);
	free(str);
	return (result);
}

/**
 * Check if an argument should be split after expansion
 */
int	should_split_arg(const char *arg)
{
	if (!arg)
		return (0);
	if (ft_strchr(arg, '$'))
	{
		if (arg[0] != '\'')
			return (1);
	}
	return (0);
}
