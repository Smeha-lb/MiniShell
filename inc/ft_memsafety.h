/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memsafety.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 15:24:35 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/25 17:51:37 by moabdels         ###   ########.fr       */
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

#endif
