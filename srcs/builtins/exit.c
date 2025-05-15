/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status;

/**
 * Check if a string is a valid number
 */
static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 * Convert a string to a long integer
 */
static long	ft_atol(const char *str)
{
	long	result;
	int		sign;
	int		i;

	result = 0;
	sign = 1;
	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

/**
 * Built-in exit command
 */
int	ft_exit(t_minishell *shell, char **args)
{
	long	exit_code;

	ft_putstr_fd("exit\n", 1);
	if (!args[1])
	{
		g_exit_status = shell->last_exit_status;
		shell->running = 0;
		exit(g_exit_status);
	}
	if (!is_valid_number(args[1]))
	{
		print_error("exit", "numeric argument required");
		g_exit_status = 255;
		shell->running = 0;
		exit(g_exit_status);
	}
	exit_code = ft_atol(args[1]);
	if (args[2])
	{
		print_error("exit", "too many arguments");
		return (1);
	}
	g_exit_status = (unsigned char)exit_code;
	shell->running = 0;
	exit(g_exit_status);
} 