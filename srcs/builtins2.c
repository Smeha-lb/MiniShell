#include "../includes/minishell.h"

static int	check_echo_flags(char **args, int *i)
{
	int	n_flag;

	n_flag = 0;
	while (args[*i] && ft_strcmp(args[*i], "-n") == 0)
	{
		n_flag = 1;
		(*i)++;
	}
	return (n_flag);
}

static void	print_echo_args(char **args, int i, int n_flag)
{
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[i + 1])
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (!n_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
}

int	builtin_echo(t_command *cmd)
{
	int	i;
	int	n_flag;

	i = 1;
	n_flag = 0;
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	n_flag = check_echo_flags(cmd->args, &i);
	print_echo_args(cmd->args, i, n_flag);
	return (0);
}
