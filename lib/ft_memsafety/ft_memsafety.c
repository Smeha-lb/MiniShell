/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memsafety.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 17:18:02 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/21 18:03:57 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memsafety.h"

// this asserts that an alignment is a power of 2
// byte operations are equivalent to modulo but faster because of power of 2s
bool	is_power_of_two(uintptr_t ptr)
{
	return ((ptr & (ptr - 1)) == 0);
}

// next, we need to find how many bytes forward we need to go for the memory
// address to be a multiple of 2
// *
uintptr_t	align_forward(uintptr_t ptr, size_t align)
{
	uintptr_t result;
	uintptr_t a;
	uintptr_t modulo;

	a = (uintptr_t)align;
	if (is_power_of_two(align))
		exit(EXIT_FAILURE);




}


