#include "../includes/minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (ft_strcmp(cmd, "echo") == 0
		|| ft_strcmp(cmd, "cd") == 0
		|| ft_strcmp(cmd, "pwd") == 0
		|| ft_strcmp(cmd, "export") == 0
		|| ft_strcmp(cmd, "unset") == 0
		|| ft_strcmp(cmd, "env") == 0
		|| ft_strcmp(cmd, "exit") == 0);
}

char	*create_heredoc_filename(int counter, pid_t pid)
{
	char	*num_str;
	char	*pid_str;
	char	*filename;

	num_str = ft_itoa(counter);
	pid_str = ft_itoa(pid);
	if (!num_str || !pid_str)
	{
		free(num_str);
		free(pid_str);
		return (NULL);
	}
	filename = build_heredoc_path(num_str, pid_str);
	free(num_str);
	free(pid_str);
	return (filename);
}
