/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_builtin.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 10:46:07 by csamaha           #+#    #+#             */
/*   Updated: 2025/04/29 17:52:19 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// Returns 0 if successful, 1 if an error occured.

// int	pwd_builtin(t_data *data, char **args)
// {
// 	char	buf[PATH_MAX];
// 	char	*cwd;

// 	(void)args;
// 	if (data->working_dir)
// 	{
// 		ft_putendl_fd(data->working_dir, STDOUT_FILENO);
// 		return (EXIT_SUCCESS);
// 	}
// 	cwd = getcwd(buf, PATH_MAX);
// 	if (cwd)
// 	{
// 		ft_putendl_fd(cwd, STDOUT_FILENO);
// 		return (EXIT_SUCCESS);
// 	}
// 	errmsg_cmd("pwd", NULL, strerror(errno), errno);
// 	return (EXIT_FAILURE);
// }
