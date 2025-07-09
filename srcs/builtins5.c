/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins5.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 15:58:46 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 15:58:47 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_valid_var_name(char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_unset(t_shell *shell, t_command *cmd)
{
	int	i;
	int	status;

	status = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_var_name(cmd->args[i]))
		{
			ft_putstr_fd("unset: `", STDERR_FILENO);
			ft_putstr_fd(cmd->args[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			status = 1;
		}
		else
			unset_env_var(shell, cmd->args[i]);
		i++;
	}
	return (status);
}

static void	update_pwd_vars(t_shell *shell)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(shell, "PWD", cwd);
}

int	builtin_cd(t_shell *shell, t_command *cmd)
{
	char	*path;
	char	cwd[1024];

	if (cmd->args[1] && cmd->args[2])
	{
		ft_putendl_fd("cd: too many arguments", STDERR_FILENO);
		return (1);
	}
	path = get_cd_path(shell, cmd->args[1]);
	if (!path)
		return (1);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(shell, "OLDPWD", cwd);
	if (chdir(path) != 0)
	{
		print_error("cd", path, strerror(errno));
		return (1);
	}
	update_pwd_vars(shell);
	return (0);
}
