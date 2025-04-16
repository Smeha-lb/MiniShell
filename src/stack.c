/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stack.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 16:01:20 by moabdels          #+#    #+#             */
/*   Updated: 2025/05/01 16:24:18 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/libft/libft.h"
#include "../inc/stack.h"

bool	stack_is_empty(t_stack *stack)
{
	return stack->top == NULL;
}

void	push(t_stack *stack, void *content)
{
	t_list	*new;

	new = malloc(sizeof(t_list*));
	if (!new)
		return (NULL);
	new->content = content;
	new->next = NULL;
	if (!stack->top)
		stack->top = new;
	else
		stack->top->next = new;
}


