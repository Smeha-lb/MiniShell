/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memsafety_one.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 17:18:02 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/25 17:43:53 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/ft_memsafety.h"

// ? the size check is necessary because malloc(0) means a specific thing

t_arena	*arena_create(size_t size)
{
	t_arena	*arena;

	if (size == 0)
		return (NULL);
	arena = malloc(sizeof(t_arena));
	if (arena == NULL)
		return (NULL);
	arena->region = malloc(size);
	if (arena->region == NULL)
	{
		free(arena);
		return (NULL);
	}

	arena->index = 0;
	arena->size = size;

	return (arena);
}
