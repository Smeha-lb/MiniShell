/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:59:32 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/24 13:01:47 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexing.h"

static bool	no_unexpected_start(char *input)
{
	t_token	token;
	
	token = parse_token(input[0], input[1]);
	if (token == OR || token == AND || token == PIPE || token == RIGHT_PAREN)
	{
		ft_printf("minishell: syntax error near unexpected token, %s" \
			, input[0]);
		return (false);
	}
	return (true);
}

static ssize_t get_right_paren_index(char *input, ssize_t i)
{
	while (*input)
	{
		if (*input == ')')
			return (i);
		i++;
		input++;
	}
	print_error("Syntax Error: Unclosed Parentheses");
	signal_handler.exit_code = EX_BAD_USAGE;
	return (-1);
}

// TODO: This can be refactored into something much nicer
static bool	is_bad_pair_p(t_token current, t_token next)
{
	if ((current == OR || current == AND || current == PIPE)
		&& (next == NOT || next == IN || next == OUT || next == APPEND
		|| next == LEFT_PAREN))
			return (false);
	if ((current == LEFT_PAREN) && (next == NOT || next == IN || next == OUT
		|| next == APPEND || next == LEFT_PAREN))
			return (false);
	if ((current == RIGHT_PAREN) && (next == AND || next == OR || next == PIPE
		|| next == RIGHT_PAREN || next == END))
			return (false);
	if ((current == APPEND || current == HEREDOC 
		|| current == IN || current == OUT) && (next == NOT))
			return (false);
	return (true);	
}

bool	is_bad_pair(t_token token, char *input)
{
	t_token	next;

	next = parse_token(*input, *(input + 1));
	if (is_bad_pair_p(token, next))
		return (true);
	if (next == END)
		ft_printf("\e[1;31minishell:\e[0m "UNEX_TOKEN"`newline`\n");
	else
		ft_printf("\e[1;31minishell:\e[0m "UNEX_TOKEN"`%c`\n", *input);
	signal_handler.exit_code = EX_BAD_USAGE;
	return (false);
}

static bool	no_token_syntax_errs(t_token token, char *input, \
		ssize_t *i, ssize_t *j_pair)
{
	(*i)++;
	if (token == OR || token == AND || token == APPEND)
		(*i)++;
	else if (token == LEFT_PAREN)
	{
		if (!*j_pair)
			*j_pair = *i;
		*j_pair = get_right_paren_index(&input[*j_pair], *j_pair) + 1;
		if (*j_pair == 0)
			return (false);
	}
	else if (token == RIGHT_PAREN && *i > *j_pair)
	{
		ft_printf("minishell: Syntax Error near unexpected token ')'\n");
		signal_handler.exit_code = EX_BAD_USAGE;
		return (false);
	}
	while (ft_iswhitespace(input[*i]))
		(*i)++;
	if (!is_bad_pair(token, &input[*i]))
		return (false);
	return (true);
}

static bool	build_tree(t_astree **root, char *input, \
		ssize_t *i, ssize_t *j_pair)
{
	t_token		token;
	t_astree	*node;

	token = parse_token(input[*i], input[(*i) + 1]);
	if (!token_is_redir(token))
	{
		if (!no_token_syntax_errs(token, input, i, j_pair))
			return (false);
		
	}
}

t_astree	*generate_tree(char *input)
{
	t_astree	*root;
	ssize_t		i;
	ssize_t		j_pair;

	j_pair = 0;
	i = 0;
	root = NULL;
	while (input[i])
		if (!build_tree(&root, input, &i, &j_pair))
			return (free_tree(root), free(input), NULL);
	free(input);
	input = NULL;
	return (root);	
}

t_astree	*generate_astree(char *user_input)
{
	char		*str;
	t_astree	*root;

	str = ft_strtrim(user_input, WHITESPACE);
	free(user_input);
	if (!str || !no_unexpected_start(str))
		return (free(str), NULL);
	root = generate_tree(str);
	root = set_exec_order(&root);
	while (root->right)
		root = root->right;
	root = prune_tree(root);	
	return (root);
}
