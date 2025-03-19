/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_char.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 13:01:17 by moabdels          #+#    #+#             */
/*   Updated: 2024/09/09 12:29:53 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

// * Flags we care about [-][width}
// we have to pass an int because of type promotion

void	print_char(t_printf_data *data, int chr)
{
	int	width;

	width = data->format.width_value;
	if (chr == '%' && data->format.specifier == '%')
	{
		putchar_n(data, '%', 1);
		return ;
	}
	if (width > 1)
	{
		if (data->format.left_justified)
		{
			putchar_n(data, (char)chr, 1);
			putchar_n(data, ' ', width - 1);
		}
		else
		{
			putchar_n(data, ' ', width - 1);
			putchar_n(data, (char)chr, 1);
		}
	}
	else
		putchar_n(data, (char)chr, 1);
}
