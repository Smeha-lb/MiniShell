/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_int.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 11:38:20 by moabdels          #+#    #+#             */
/*   Updated: 2024/07/15 13:44:59 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void	put_sign(t_printf_data *data)
{
	if (data->format.base == BASE_16 && check_prepend(data))
	{
		if (data->format.upper_case)
			putstr_n(data, "0X", 2);
		else
			putstr_n(data, "0x", 2);
		return ;
	}
	if (!data->format.signed_value)
		return ;
	if (data->format.is_negative)
	{
		putchar_n(data, '-', 1);
		return ;
	}
	if (data->format.plus)
		putchar_n(data, '+', 1);
	else if (data->format.space)
		putchar_n(data, ' ', 1);
}

static void	itoa_buf_helper(t_printf_data *data, t_u_int_wrapper int_values)
{
	t_u_int_wrapper	temp;

	if (data->format.is_negative && !data->format.is_converted)
	{
		int_values.int64 = -(int_values.int64);
		data->format.is_converted = 1;
		itoa_buf_helper(data, int_values);
	}
	else if (int_values.uint64 < data->format.base)
	{
		if (data->format.upper_case)
			data->format.nbr_container[data->format.nbr_len++] = \
			PRINTF_SYMBOLS_UP[int_values.uint64];
		else
			data->format.nbr_container[data->format.nbr_len++] = \
			PRINTF_SYMBOLS_LOW[int_values.uint64];
	}
	else
	{
		temp.uint64 = int_values.uint64 / data->format.base;
		itoa_buf_helper(data, temp);
		temp.uint64 = int_values.uint64 % data->format.base;
		itoa_buf_helper(data, temp);
	}
}

void	itoa_buf(t_printf_data *data, t_u_int_wrapper int_values)
{
	if (data->format.base < 2 || data->format.base > 16)
		return ;
	itoa_buf_helper(data, int_values);
}

// * Flags we care about [+ 0#-][width][.precision]
// * Number of 0s is derived from : .precision, the 0 flag, and nbr_len

void	print_int(t_printf_data *data, t_u_int_wrapper int_values)
{
	itoa_buf(data, int_values);
	if (data->format.specifier == 'p' && int_values.uint64 == 0)
	{
		print_str(data, "(nil)");
		return ;
	}
	if (data->format.precision_value == 0 && int_values.uint64 == 0)
	{
		ft_memset(data->format.nbr_container, 0, 1);
		data->format.nbr_len--;
	}
	set_padding_zeros(data);
	set_padding_spaces(data);
	if (!data->format.left_justified)
		putchar_n(data, ' ', data->format.padding_spaces);
	put_sign(data);
	putchar_n(data, '0', data->format.padding_zeros);
	putstr_n(data, data->format.nbr_container, data->format.nbr_len);
	if (data->format.left_justified)
		putchar_n(data, ' ', data->format.padding_spaces);
}
