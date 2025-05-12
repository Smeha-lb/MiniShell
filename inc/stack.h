/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:02:30 by moabdels          #+#    #+#             */
/*   Updated: 2025/05/01 16:15:29 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STACK_H
#define STACK_H

# include "../lib/libft/libft.h"

typedef struct s_stack
{
	t_list	*top;
} t_stack;

bool	stack_is_empty(t_stack *stack);
void	push(t_stack *stack, void *value);
void	*pop(t_stack *stack);

void	stack_free(t_stack *stack, void (*del)(void*));
void	stack_iter(t_list *lst, void (*f)(void*));
t_stack	*stack_map(t_stack *stack, void *(*f)(void*), void (*del)(void *));

#endif