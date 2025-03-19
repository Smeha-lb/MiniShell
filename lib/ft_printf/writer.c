/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   writer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 12:22:02 by moabdels          #+#    #+#             */
/*   Updated: 2024/09/09 12:33:00 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
// * handle padding

void	putchar_n(t_printf_data *data, char chr, int precision)
{
	if (precision <= 0)
		return ;
	while (precision--)
		write_data(data, chr);
}

void	putstr_n(t_printf_data *data, char *str, int precision)
{
	if (precision <= 0 || !str)
		return ;
	while (precision-- && *str)
		write_data(data, *str++);
}

void	write_data(t_printf_data *data, char chr)
{
	data->chars_written += write(STDOUT_FILENO, &chr, 1);
}
