/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:45:56 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/30 19:08:19 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include <math.h>

static const char *token_to_str(t_token token)
{
    switch (token)
    {
        case NOT:          return "NOT";
        case PIPE:        return "PIPE";
        case HEREDOC:      return "HEREDOC";
        case LEFT_PAREN:  return "LEFT_PAREN";
        case RIGHT_PAREN:  return "RIGHT_PAREN";
        case AND:         return "AND";
        case OR:          return "OR";
        case APPEND:      return "APPEND";
        case OUT:         return "OUT";
        case IN:          return "IN";
        case END:         return "END";
        default:         return "ERR?";
    }
}

// ? n is the number of siblings printed so far
void	print_node(t_astree *node, int depth, bool nl)
{
	printf("%-*s", depth * 4, token_to_str(node->token));
	if (nl)
		printf("/n");
}

void	print_astree_p(t_astree *node, int depth, bool nl)
{
	if (depth == 0)
		return ;
	print_node(node, depth, nl);
	print_astree_p(node->left, depth - 1, false);
	print_astree_p(node->right, depth - 1, true);
}

int	get_tree_depth_n(t_astree *node, int depth)
{
	printf("get_tree_depth (%p, %d)\n", node, depth);
	if (!node || (!node->left && !node->right))
		return (depth);
	if (!node->left)
		return (fmax(-1, get_tree_depth_n(node->right, depth + 1)));
	if (!node->right)
		return (fmax(get_tree_depth_n(node->left, depth + 1), -1));
	return fmax(get_tree_depth_n(node->left, depth + 1), \
		get_tree_depth_n(node->right, depth + 1));
}

int	get_tree_depth(t_astree *root)
{
	return (get_tree_depth_n(root, 0));
}

void	print_astree(t_astree *root)
{
	printf("tree depth is <%d>", get_tree_depth(root));
	// ! test node printing
}

int	main(void)
{
	t_astree	*root, *L, *R, *LR, *LRL, *LRR, *LRRL, *RR, *RRL;
	root = malloc(sizeof(t_astree));
	L = malloc(sizeof(t_astree));
	R = malloc(sizeof(t_astree));
	LR = malloc(sizeof(t_astree));
	RR = malloc(sizeof(t_astree));
	RRL = malloc(sizeof(t_astree));
	LRL = malloc(sizeof(t_astree));
	LRR = malloc(sizeof(t_astree));
	LRRL = malloc(sizeof(t_astree));
	root->left = L;
	root->right = R;
	L->right = LR;
	L->left = NULL;
	R->right = RR;
	R->left = NULL;
	RR->left = RRL;
	RR->right = NULL;
	LR->left = LRL;
	LR->right = LRR;
	LRR->left = LRRL;
	LRR->right = NULL;

	print_astree(root);
	exit(OK);
}