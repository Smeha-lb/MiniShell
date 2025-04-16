/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:59:32 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/16 17:00:37 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexing.h"

static void ft_iswhitespace(char *c)
{
	
}

void    extract_lexemes(t_list **list, char *user_input)
{
	int		i;
	char	*lexeme;

	i = 0;
	while (user_input[i])
	{
		if (ft_is)
	}
	
}

void    generate_tokens(t_minishell *minishell)
{
	t_list  *temp;

	temp = NULL;
	extract_lexemes(&temp, minishell->user_input);
	// TODO: Err handling
}


