/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 16:03:21 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/24 14:18:00 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include "../libft/libft.h"
# include <readline/readline.h>

# define PRINTF_FLAGS	"+0 -#"
# define PRINTF_SPECIFIERS	"cspdiuxX%"
# define PRINTF_SYMBOLS_LOW "0123456789abcdef"
# define PRINTF_SYMBOLS_UP "0123456789ABCDEF"

typedef union u_int_wrapper
{
	unsigned long	uint64;
	long			int64;
}	t_u_int_wrapper;

typedef enum e_printf_error
{
	PRINTF_OK = 0,
	PRINTF_MALLOC_ERROR = -1,
	PRINTF_PARSE_ERROR = -2
}	t_e_printf_error;

typedef enum e_base
{
	BASE_2 = 2,
	BASE_8 = 8,
	BASE_10 = 10,
	BASE_16 = 16
}	t_e_base;

typedef struct s_format
{
	int			left_justified;
	int			plus;
	int			space;
	int			hash;
	int			zero_pad;

	char		specifier;

	int			width_value;
	int			precision_value;

	int			padding_spaces;
	int			padding_zeros;

	t_e_base	base;
	int			upper_case;
	int			signed_value;
	int			is_negative;
	int			is_converted;

	char		nbr_container[64];
	int			nbr_len;
}	t_format;

typedef struct s_printf_data
{
	const char	*str;
	va_list		ap;
	int			fd;
	int			chars_written;
	t_format	format;
}	t_printf_data;

void	*ft_calloc(size_t nmemb, size_t size);

int		parse_format(t_printf_data *data);

void	write_data(t_printf_data *data, char chr);
void	putchar_n(t_printf_data *data, char chr, int precision);
void	putstr_n(t_printf_data *data, char *str, int precision);
void	print_char(t_printf_data *data, int chr);
void	itoa_buf(t_printf_data *data, t_u_int_wrapper int_values);

int		check_prepend(t_printf_data *data);
void	put_sign(t_printf_data *data);
void	set_padding_spaces(t_printf_data *data);
void	set_padding_zeros(t_printf_data *data);
void	print_str(t_printf_data *data, char *str);
void	print_int(t_printf_data *data, t_u_int_wrapper int_values);

int		ft_printf(const char *format, ...);
int		ft_dprintf(int fd, const char *format, ...);

#endif
