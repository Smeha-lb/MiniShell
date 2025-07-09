/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csamaha <csamaha@student.42beirut.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 16:14:09 by csamaha           #+#    #+#             */
/*   Updated: 2025/07/09 16:14:10 by csamaha          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	has_wildcards(const char *str)
{
	int	in_single_quotes;
	int	in_double_quotes;

	if (!str)
		return (0);
	in_single_quotes = 0;
	in_double_quotes = 0;
	while (*str)
	{
		if (*str == '\'' && !in_double_quotes)
			in_single_quotes = !in_single_quotes;
		else if (*str == '\"' && !in_single_quotes)
			in_double_quotes = !in_double_quotes;
		else if (!in_single_quotes && (*str == '*'
				|| *str == '?' || *str == '['))
			return (1);
		str++;
	}
	return (0);
}

static int	check_char(char c, const char *class_str, int negate)
{
	if (c == *class_str)
	{
		if (negate)
			return (0);
		else
			return (1);
	}
	return (-1);
}

static int	check_range(char c, const char *class_str, int negate)
{
	if (c >= class_str[0] && c <= class_str[2])
	{
		if (negate)
			return (0);
		else
			return (1);
	}
	return (-1);
}

static int	process_class_char(char c, const char **class_str, int negate)
{
	int	result;

	if ((*class_str)[0] != '-' && (*class_str)[1] == '-' &&
			(*class_str)[2] != ']')
	{
		result = check_range(c, *class_str, negate);
		if (result >= 0)
			return (result);
		*class_str += 3;
	}
	else
	{
		result = check_char(c, *class_str, negate);
		if (result >= 0)
			return (result);
		(*class_str)++;
	}
	return (-1);
}

int	is_in_char_class(char c, const char *class_str)
{
	int	negate;
	int	result;

	class_str++;
	negate = 0;
	if (*class_str == '!')
	{
		negate = 1;
		class_str++;
	}
	while (*class_str && *class_str != ']')
	{
		result = process_class_char(c, &class_str, negate);
		if (result >= 0)
			return (result);
	}
	if (negate)
		return (1);
	else
		return (0);
}
