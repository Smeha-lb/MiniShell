/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:13:40 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:13:41 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	clean_tokens(t_shell *shell)
{
	shell->exit_status = 1;
	free_tokens(shell->tokens);
	shell->tokens = NULL;
}

void	shell_init(t_shell *shell, char **env)
{
	char	*shlvl_value;
	int		shlvl;
	char	*new_shlvl;

	if (!env || !env[0])
		initialize_minimal_env(shell);
	else
		shell->env = copy_env(env);
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->exit_status = 0;
	shell->running = 1;
	shell->previous_cmd = NULL;
	shlvl_value = get_env_value(shell, "SHLVL");
	if (shlvl_value)
		shlvl = ft_atoi(shlvl_value);
	else
		shlvl = 0;
	shlvl++;
	new_shlvl = ft_itoa(shlvl);
	if (new_shlvl)
	{
		set_env_var(shell, "SHLVL", new_shlvl);
		free(new_shlvl);
	}
}
