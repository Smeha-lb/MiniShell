/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memsafety.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 15:24:35 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/25 17:34:58 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MEMSAFETY_H
# define FT_MEMSAFETY_H

# include <stdarg.h>
# include <stdint.h>
# include <sys/types.h>
# include <stdlib.h>
# include <stdbool.h>
# include "../lib/ft_printf/ft_printf.h"
# include "../lib/libft/libft.h"

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void  *))
#endif

// ! How to Define Arrays
// typedef struct {
// 	int*	items;
// 	int		length;
// 	int		capacity;
// }	IntArray

// ! Then, define a Getter function, this avoids messing up array bounds.
// int	IntArray_Get(IntArray array, int index)
// {
// 	if (index >= 0 && index < array.length)
// 		return array.items[index];
// 	return 0; // some default value that makes sense or something like
//				raise(SIGTRAP)
// }
// ? you don't have to worry about performance. the compiler is smart enough
// ? to optimize the if statement out

// ! Storing references to other objects:
// ? If you're referencing an object in an array don't include the
// ? pointer itself as a field if possible, include the index in
// ? that array, so for example:

// typedef struct User
//{
//	String name;
//	int friendIndex; // the index of User in the friends array of other user
//}

typedef struct s_string
{
	char*	chars;
	int		length;
}	t_string;

typedef struct s_arena
{
	char	*region;
	size_t	index;
	size_t	size;
}	t_arena;


/*
* Allocate and return a pointer to memory to the arena with a region with the
* specified size. Providing a size of zero results in an error

Parameters:
  size_t size	|	The size (in bytes) of the arena
					  memory region.
Return:
  Pointer to arena on success, NULL on failure

*/
t_arena	*arena_create(size_t size);

/*
* Reallocate an arena's region to a greater or equal size.
* Returns the realloc'd arena on success, and NULL on failure.
* Passing a null arena, providing a size less than or equal
* to the arena's current size, or a failed realloc call will
* all result in returning NULL.

Parameters
  t_arena *arena    |    The arena whose region is being
                       realloc'd
  size_t size     |    The size the arena's region is
                       being changed to
*/

t_arena*	arena_expand(t_arena *arena, size_t size);

/*
* Return a pointer to a portion of specified size of the
specified arena's region. Nothing will restrict you
from allocating more memory than you specified, so be
mindful of your memory (as you should anyways) or you
will get some hard-to-track bugs. By default, memory is
aligned by alignof(size_t), but you can change this by
#defining ARENA_DEFAULT_ALIGNMENT before #include'ing
arena.h. Providing a size of zero results in a failure.

Parameters:
  t_arena *arena    |    The arena of which the pointer
                       from the region will be
                       distributed
  size_t size     |    The size (in bytes) of
                       allocated memory planned to be
                       used.
Return:
  Pointer to arena region segment on success, NULL on
  failure.
*/
void*	arena_alloc(t_arena *arena, size_t size);


/*
Same as arena_alloc, except you can specify a memory
alignment for allocations.

Return a pointer to a portion of specified size of the
specified arena's region. Nothing will restrict you
from allocating more memory than you specified, so be
mindful of your memory (as you should anyways) or you
will get some hard-to-track bugs. Providing a size of
zero results in a failure.

Parameters:
  t_arena *arena              |    The arena of which the pointer
                                 from the region will be
                                 distributed
  size_t size               |    The size (in bytes) of
                                 allocated memory planned to be
                                 used.
  unsigned int alignment    |    Alignment (in bytes) for each
                                 memory allocation.
Return:
  Pointer to arena region segment on success, NULL on
  failure.
*/
void*	arena_alloc_aligned(t_arena *arena, size_t size, unsigned int alignment);


/*
Copy the memory contents of one arena to another.

Parameters:
  t_arena *src     |    The arena being copied, the source.
  t_arena *dest    |    The arena being copied to. Must be created/allocated
                      already.

Return:
  Number of bytes copied.
*/
size_t	arena_copy(t_arena *dest, t_arena *src);


/*
Reset the pointer to the arena region to the beginning
of the allocation. Allows reuse of the memory without
realloc or frees.

Parameters:
  t_arena *arena    |    The arena to be cleared.
*/
void	arena_clear(t_arena* arena);


/*
Free the memory allocated for the entire arena region.

Parameters:
  t_arena *arena    |    The arena to be destroyed.
*/
void	arena_destroy(t_arena *arena);

#endif
