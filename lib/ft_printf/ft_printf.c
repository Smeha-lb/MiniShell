/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 11:32:08 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/24 14:24:34 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

// * Wrapper for all rendering functions
void	render_format(t_printf_data *data)
{
	t_u_int_wrapper	int_values;

	int_values.int64 = 0;
	if (data->format.specifier == '%')
		print_char(data, '%');
	else if (data->format.specifier == 'c')
		print_char(data, va_arg(data->ap, int));
	else if (data->format.specifier == 's')
		print_str(data, va_arg(data->ap, char *));
	else if (ft_strchr("dipxXu", data->format.specifier))
	{
		if (ft_strchr("di", data->format.specifier))
		{
			int_values.int64 = (long)va_arg(data->ap, int);
			data->format.signed_value = 1;
			if (int_values.int64 < 0)
				data->format.is_negative = 1;
		}
		else if (data->format.specifier == 'p')
			int_values.uint64 = (unsigned long)va_arg(data->ap, void *);
		else if (ft_strchr("xXu", data->format.specifier))
			int_values.uint64 = (unsigned long)va_arg(data->ap, unsigned int);
		print_int(data, int_values);
	}
}

// in case I want to use a buffer later I'll keep this

static int	init_data(t_printf_data *data, const char *format)
{
	data->chars_written = 0;
	data->str = format;
	return (PRINTF_OK);
}

int	ft_printf(const char *format, ...)
{
	t_printf_data	data;

	va_start(data.ap, format);
	if (init_data(&data, format))
		return (PRINTF_MALLOC_ERROR);
	data.fd = STDIN_FILENO;
	while (*data.str)
	{
		if (*data.str == '%' && *(++data.str))
		{
			parse_format(&data);
			render_format(&data);
		}
		else
			write_data(&data, *data.str);
		data.str++;
	}
	va_end(data.ap);
	return (data.chars_written);
}

int	ft_dprintf(int fd, const char *format, ...)
{
	t_printf_data	data;

	va_start(data.ap, format);
	if (init_data(&data, format))
		return (PRINTF_MALLOC_ERROR);
	data.fd = fd;
	while (*data.str)
	{
		if (*data.str == '%' && *(++data.str))
		{
			parse_format(&data);
			render_format(&data);
		}
		else
			write_data(&data, *data.str);
		data.str++;
	}
	va_end(data.ap);
	return (data.chars_written);
}