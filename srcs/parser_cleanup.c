/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cleanup.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:11:52 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:11:53 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_redirs(t_redir *redirs)
{
	t_redir	*temp;

	while (redirs)
	{
		temp = redirs;
		redirs = redirs->next;
		free(temp->file);
		free(temp);
	}
}

void	free_cmd_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_commands(t_command *commands)
{
	t_command	*temp;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		if (temp->is_subshell && temp->subshell)
			free_commands(temp->subshell);
		free_cmd_args(temp->args);
		free(temp->arg_quoted);
		free_redirs(temp->redirs);
		free(temp);
	}
}
