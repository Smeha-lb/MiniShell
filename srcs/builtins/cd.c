/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Update PWD and OLDPWD environment variables
 */
static void	update_pwd(t_minishell *shell)
{
	char	*old_pwd;
	char	cwd[4096];

	old_pwd = get_env_value(shell->env, "PWD");
	if (old_pwd)
		set_env_var(&shell->env, "OLDPWD", old_pwd);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(&shell->env, "PWD", cwd);
}

/**
 * Built-in cd command
 */
int	ft_cd(t_minishell *shell, char **args)
{
	char	*path;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		path = get_env_value(shell->env, "HOME");
		if (!path)
		{
			print_error("cd", "HOME not set");
			return (1);
		}
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = get_env_value(shell->env, "OLDPWD");
		if (!path)
		{
			print_error("cd", "OLDPWD not set");
			return (1);
		}
		ft_putstr_fd(path, 1);
		ft_putstr_fd("\n", 1);
	}
	else
		path = args[1];
	if (chdir(path) != 0)
	{
		print_error("cd", NULL);
		return (1);
	}
	update_pwd(shell);
	return (0);
} 