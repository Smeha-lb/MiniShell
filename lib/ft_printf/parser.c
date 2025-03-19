/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 13:43:35 by moabdels          #+#    #+#             */
/*   Updated: 2024/09/09 12:32:45 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	parse_padding(t_printf_data *data)
{
	int	value;

	value = 0;
	while (ft_isdigit(*data->str))
		value = (value * 10) + (*data->str++ - '0');
	return (value);
}

static void	parse_flags(t_printf_data *data)
{
	char	flag;

	while (ft_strchr(PRINTF_FLAGS, *data->str))
	{
		flag = *data->str;
		if (flag == '0')
			data->format.zero_pad = 1;
		else if (flag == '+')
			data->format.plus = 1;
		else if (flag == ' ')
			data->format.space = 1;
		else if (flag == '#')
			data->format.hash = 1;
		else if (flag == '-')
			data->format.left_justified = 1;
		++data->str;
	}
}

static void	get_value(t_printf_data *data, int *value)
{
	if (*data->str == '*')
	{
		*value = va_arg(data->ap, int);
		++data->str;
		return ;
	}
	*value = parse_padding(data);
}

// fixing numbers : this executes fine
int	parse_format(t_printf_data *data)
{
	ft_memset(&data->format, 0, sizeof(t_format));
	data->format.precision_value = -1;
	parse_flags(data);
	get_value(data, &data->format.width_value);
	if (*data->str == '.' && *(++data->str))
	{
		get_value(data, &data->format.precision_value);
		data->format.zero_pad = 0;
	}
	if (ft_strchr(PRINTF_SPECIFIERS, *data->str))
	{
		data->format.specifier = *data->str;
		if (ft_strchr("diu", data->format.specifier))
			data->format.base = BASE_10;
		else if (ft_strchr("xXp", data->format.specifier))
		{
			data->format.base = BASE_16;
			if ('X' == data->format.specifier)
				data->format.upper_case = 1;
		}
	}
	else
		return (PRINTF_PARSE_ERROR);
	return (PRINTF_OK);
}
