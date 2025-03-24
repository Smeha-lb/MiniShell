/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memsafety.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 15:24:35 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/24 15:08:14 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MEMSAFETY_H
# define FT_MEMSAFETY_H

# include <stdarg.h>
# include <stdint.h>
# include <sys/types.h>
# include <stdlib.h>
# include <stdbool.h>

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

typedef struct String
{
	char*	chars;
	int		length;
}	String;

bool	is_power_of_two(uintptr_t ptr);

#endif
