/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 12:53:23 by moabdels          #+#    #+#             */
/*   Updated: 2024/07/15 13:44:59 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	check_prepend(t_printf_data *data)
{
	return ((ft_strchr("xX", data->format.specifier) && data->format.hash
			&& data->format.nbr_container[0] != '0')
		|| data->format.specifier == 'p');
}

void	set_padding_zeros(t_printf_data *data)
{
	int		nbr_len;

	nbr_len = data->format.nbr_len;
	if (data->format.precision_value >= 0
		&& data->format.precision_value > nbr_len)
	{
		data->format.padding_zeros = data->format.precision_value - nbr_len;
		return ;
	}
	if (data->format.zero_pad && !data->format.left_justified)
	{
		if (data->format.width_value > nbr_len)
			data->format.padding_zeros = data->format.width_value - nbr_len;
		if (data->format.specifier == 'u')
			return ;
		else if (check_prepend(data))
			data->format.padding_zeros -= 2;
		else if (data->format.is_negative || data->format.plus
			|| data->format.space)
			data->format.padding_zeros--;
	}
	if (data->format.padding_zeros < 0)
		data->format.padding_zeros = 0;
}

void	set_padding_spaces(t_printf_data *data)
{
	data->format.padding_spaces = data->format.width_value - \
		data->format.padding_zeros - data->format.nbr_len;
	if (check_prepend(data))
	{
		data->format.padding_spaces -= 2;
		return ;
	}
	if (data->format.is_negative || data->format.plus || data->format.space)
		data->format.padding_spaces--;
}
