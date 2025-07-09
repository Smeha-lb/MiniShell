/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_pipeline_utils3.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:00:08 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:00:09 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_pipes(int **pipes, int cmd_count)
{
	int	i;

	i = 0;
	while (i < cmd_count - 1)
	{
		if (pipes[i][0] >= 0)
			close(pipes[i][0]);
		if (pipes[i][1] >= 0)
			close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}
