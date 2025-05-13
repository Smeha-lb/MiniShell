/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/09 11:49:17 by moabdels          #+#    #+#             */
/*   Updated: 2024/07/15 16:44:17 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	set_str_padding_spaces(t_printf_data *data, char *str)
{
	int	str_len;
	int	precision;
	int	width;

	if (str)
		str_len = ft_strlen(str);
	else
		str_len = 0;
	precision = data->format.precision_value;
	width = data->format.width_value;
	if (width <= 0)
		return ;
	if (precision >= 0)
	{
		if (precision >= str_len)
			data->format.padding_spaces = width - str_len;
		else if (precision < str_len)
			data->format.padding_spaces = width - precision;
	}
	else
		data->format.padding_spaces = width - str_len;
}

/**
 * flags we care about
 * [-] [width][.precision]
 * handle justicification
 * handle width
 * print precision - init data doesn't do anything to
 *
 */

void	print_str(t_printf_data *data, char *str)
{
	int	precision;

	precision = data->format.precision_value;
	if (!str)
	{
		if (precision >= 0 && precision < 6)
			str = "\0";
		else
			str = "(null)";
	}
	set_str_padding_spaces(data, str);
	if (!data->format.left_justified)
		putchar_n(data, ' ', data->format.padding_spaces);
	if (precision >= 0)
		putstr_n(data, str, precision);
	else
		putstr_n(data, str, ft_strlen(str));
	if (data->format.left_justified)
		putchar_n(data, ' ', data->format.padding_spaces);
}
