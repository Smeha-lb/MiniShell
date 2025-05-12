/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/30 15:45:56 by moabdels          #+#    #+#             */
/*   Updated: 2025/05/01 15:47:39 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include <math.h>

// static const char *token_to_str(t_token token)
// {
//     switch (token)
//     {
//         case NOT:          return "NOT";
//         case PIPE:        return "PIPE";
//         case HEREDOC:      return "HEREDOC";
//         case LEFT_PAREN:  return "LEFT_PAREN";
//         case RIGHT_PAREN:  return "RIGHT_PAREN";
//         case AND:         return "AND";
//         case OR:          return "OR";
//         case APPEND:      return "APPEND";
//         case OUT:         return "OUT";
//         case IN:          return "IN";
//         case END:         return "END";
//         default:         return "ERR?";
//     }
// }

// // ? n is the number of siblings printed so far
// void	print_node(t_astree *node, int depth, bool nl)
// {
// 	if (!node)
// 		return ;
// 	printf("%*s", depth * 4, token_to_str(node->token));
// 	if (nl)
// 		printf("\n");
// }

// void	print_astree_r(t_astree *node, int depth, bool is_right)
// {
// 	if (!node || depth == 0)
// 		return ;

// 		print_node(node, depth, is_right);
// 		print_astree_r(node->right, depth - 1, true);
// 		print_astree_r(node->left, depth - 1, false);
// }

// int	get_tree_depth_r(t_astree *node, int depth)
// {
// 	if (!node || (!node->left && !node->right))
// 		return (depth);
// 	if (!node->left)
// 		return (fmax(-1, get_tree_depth_r(node->right, depth + 1)));
// 	if (!node->right)
// 		return (fmax(get_tree_depth_r(node->left, depth + 1), -1));
// 	return fmax(get_tree_depth_r(node->left, depth + 1), \
// 		get_tree_depth_r(node->right, depth + 1));
// }

// int	get_tree_depth(t_astree *root)
// {
// 	if (!root)
// 		return (-1);
// 	return (get_tree_depth_r(root, 1));
// }

// void	print_astree(t_astree *root)
// {

// 	int	depth;

// 	if (!root)
// 		return ;
// 	depth = get_tree_depth(root);
// 	print_astree_r(root, depth, true);
// }

// int	main(void)
// {
// 	t_astree	*root, *L, *R, *LR, *LRL, *LRR, *LRRL, *RR, *RRL;
// 	root = malloc(sizeof(t_astree));
// 	L = malloc(sizeof(t_astree));
// 	R = malloc(sizeof(t_astree));
// 	LR = malloc(sizeof(t_astree));
// 	RR = malloc(sizeof(t_astree));
// 	RRL = malloc(sizeof(t_astree));
// 	LRL = malloc(sizeof(t_astree));
// 	LRR = malloc(sizeof(t_astree));
// 	LRRL = malloc(sizeof(t_astree));
// 	root->left = L;
// 	root->right = R;
// 	root->token = HEREDOC;
// 	L->right = LR;
// 	L->left = NULL;
// 	L->token = LEFT_PAREN;
// 	R->right = RR;
// 	R->left = NULL;
// 	R->token = RIGHT_PAREN;
// 	RR->left = RRL;
// 	RR->right = NULL;
// 	RR->token = APPEND;
// 	LR->left = LRL;
// 	LR->right = LRR;
// 	LR->token = IN;
// 	LRL->left = NULL;
// 	LRL->right = NULL;
// 	LRL->token = OR;
// 	LRR->left = LRRL;
// 	LRR->right = NULL;
// 	LRR->token = PIPE;
// 	RRL->left = NULL;
// 	RRL->right = NULL;
// 	RRL->token = AND;
// 	LRRL->token = NOT;
// 	LRRL->left = NULL;
// 	LRRL->right = NULL;


// 	print_astree(root);
// 	exit(OK);
// }