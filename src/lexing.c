/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:59:32 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/30 14:14:48 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/lexing.h"
#include "../inc/minishell.h"

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
	exit_on_err(EX_BAD_USAGE, "Syntax Error - Unclosed Parentheses");
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
		ft_printf("\e[1;31mminishell:\e[0m "UNEX_TOKEN"`newline`\n");
	else
		ft_printf("\e[1;31mminishell:\e[0m "UNEX_TOKEN"`%c`\n", *input);
	signal_handler.exit_code = EX_BAD_USAGE;
	return (false);
}

static bool	no_token_syntax_errs(t_token token, char *input, ssize_t *i, ssize_t *j_pair)
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
		ft_printf(MSH_ERR"Syntax Error near unexpected token ')'\n");
		signal_handler.exit_code = EX_BAD_USAGE;
		return (false);
	}
	while (ft_iswhitespace(input[*i]))
		(*i)++;
	if (!is_bad_pair(token, &input[*i]))
		return (false);
	return (true);
}

t_astree	*create_tree_node(char *input, t_redirect *redirect, \
	t_token token, int precedence)
{
	t_astree	*node;

	node = malloc(sizeof(t_astree));
	// TODO: change below to use ft_dprintf
	if (!node)
		return (exit_on_err(1, strerror(errno)), NULL);
	node->token = token;
	node->left = NULL;
	node->right = NULL;
	node->prev_cmd = input;
	node->redir_tree = redirect;
	node->precedence = precedence;
	node->cmd = NULL;
	return (node);
}

t_redirect	*create_redir_node(char **str, t_token token)
{
	t_redirect	*node;

	if (!str)
		return (NULL);
	node = malloc(sizeof(t_redirect));
	if (!node)
	{
		exit_on_err(errno, "Malloc Error @ create_redir_node");
		return (NULL);
	}
	node->token = token;
	node->file = str[0];
	node->flag = 1;
	node->left = NULL;
	node->right = NULL;
	free(str);
	str = NULL;
	return (node);
}

bool	choose_str(char c, bool flag)
{
	return ((flag && parse_token(c, 0)) == NOT)
		|| (!flag && parse_token(c, 0) == NOT && !ft_iswhitespace(c));
}

//TODO: might be able to refactor this based on
//TODO: how other functions are calling it
// ? wtf is going on here
ssize_t	get_closing_quote(char *input, char chr)
{
	int	i;

	i = 0;
	while (*input)
	{
		if (*input == chr)
			return (i);
		i++;
		input++;
	}
	exit_on_err(EX_BAD_USAGE, MSH_ERR"Unclosed Quotes");
	return (-1);
}

ssize_t	get_cmd_len(char *input, ssize_t i, ssize_t flag)
{
	ssize_t len;
	ssize_t	keep;

	len = 0;
	while (input[i] && choose_str(input[i], flag))
	{
		if (input[i] == '"' || input[i] == '\'')
		{
			keep = get_closing_quote(&input[i + 1], input[i]);
			if (keep == -1)
				return (-1);
			len += keep + 1;
			i += keep + 1;
		}
		len++;
		i++;
	}
	if (!flag)
		while(ft_iswhitespace(input[i++]))
			len++;
	return (len);
}

//TODO: refactor this ugliness 🤢
char	*get_cmd_p(char *input, ssize_t len, ssize_t *start, bool flag)
{
	ssize_t i;
	ssize_t j;
	char	*result;

	i = 0;
	result = ft_calloc(len + 1, sizeof(char));
	while (i < len)
	{
		if (input[*start] == '"' ||  input[*start] == '\'')
		{
			j = get_closing_quote(&input[(*start) + 1], input[*start]) + i + 1;
			while (i < j)
			{
				result[i++] = input[(*start)++];
				if (!flag)
					input[(*start) - 1] = DEL;
			}
		}
		if (input[*start] == ' ')
			input[*start] = DEL;
		result[i++] = input[(*start)++];
		if (!flag)
			input[(*start) - 1] = DEL;
	}
	result[i] = 0;
	return (result);
}

char	**get_cmd(char *input, ssize_t len, ssize_t *start, bool flag)
{
	char	*cmd_str;
	char	**result;

	if (len < 0)
		return (NULL);

	cmd_str = get_cmd_p(input, len, start, flag);
	result = ft_split(cmd_str, DEL);
	free(cmd_str);
	cmd_str = NULL;
	return (result);
}

char	**format_cmd_string(char *input, ssize_t *i, ssize_t flag, t_token token)
{
	ssize_t	len;

	input[*i] = DEL;
	(*i)++;
	if (token == HEREDOC || token == APPEND)
	{
		input[*i] = DEL;
		(*i)++;
	}
	while (ft_iswhitespace(input[*i]))
		(*i)++;
	if (!is_bad_pair(token, &input[*i]))
		return (NULL);
	len = get_cmd_len(input, *i, flag);
	return (get_cmd(input, len, i, flag));
}

// static void	incre(char *input, ssize_t *i)
// {
// 	ssize_t len;
// 	len = get_cmd_len(input, *i, true);
// 	while(len--)
// 		(*i)++;
// }

bool	append_redir_node(t_redirect **root, t_redirect *new)
{
	t_redirect	*temp;

	if (!root || !new)
		return (false);
	if (!*root)
	{
		*root = new;
		return (true);
	}
	temp = *root;
	while (temp->right)
		temp = temp->right;
	temp->right = new;
	new->left = temp;
	return (true);
}

// TODO: HOLY SHIT PLEASE REFACTOR
t_astree	*build_tree_p(char *input, t_token token, ssize_t *i)
{
	ssize_t		temp;
	t_redirect	*root;
	char		*str;
	char		**cmd;

	temp = *i;
	root = NULL;
	while (token_is_redir(token))
	{
		if (token != NOT)
		{
			cmd = format_cmd_string(input, &temp, false, token);
			if (!append_redir_node(&root, create_redir_node(cmd, token)))
				return (NULL);
		}
		else
			temp++;
		token = parse_token(input[temp], input[temp + 1]);
	}
	temp = get_cmd_len(input, *i, true);
	if (temp < 0)
		return (NULL);
	str = ft_substr(input, *i, temp);
	if (!str)
		return (NULL);
	while (temp--)
		(*i)++;
	return (create_tree_node(str, root, NOT, 0));
}

bool	append_astree_node(t_astree **root, t_astree *new)
{
	t_astree	*temp;

	if (!root || !new)
		return (false);
	if (!*root)
	{
		*root = new;
		return (true);
	}
	temp = *root;
	while (temp->right)
		temp = temp->right;
	temp->right = new;
	new->left = temp;
	return (true);
}

static bool	paren_is_closed(t_astree **root)
{
	if (!root || !*root)
		return (true);
	while (*root && (*root)->right)
		*root = (*root)->right;
	if ((*root)->token == NOT)
		return (exit_on_err(EX_BAD_USAGE, MSH_ERR""UNEX_TOKEN"'('\n"), false);
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
		// TODO: make it so that we don't call get_precedence here?
		node = create_tree_node(NULL, NULL, token, get_token_precedence(token));
	}
	else
		node = build_tree_p(input, token, i);
	if (!node)
		return (false);
	if (node->token == LEFT_PAREN && !paren_is_closed(root))
		return (free(node), false);
	return (true);
}

void	free_redir_tree(t_redirect *node)
{
	t_redirect *temp;
	while (node)
	{
		free(node->file);
		temp = node;
		node = node->right;
		free(temp);
	}
}

void	free_astree(t_astree *root)
{
	t_astree *temp;
	while (root)
	{
		if (root->prev_cmd)
			free(root->prev_cmd);
		if (root->redir_tree)
			free_redir_tree(root->redir_tree);
		temp = root;
		root = root->right;
		free(temp);
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
			return (free_astree(root), free(input), NULL);
	free(input);
	input = NULL;
	return (root);
}

// TODO: rename flag to what it actually represents
static void	shunt_push(t_astree **root_a, t_astree **root_b, bool flag)
{
	t_astree	*temp;

	if (!root_a || (*root_a))
		return ;
	temp = *root_a;
	*root_a = (*root_a)->right;
	temp->right = NULL;
	if (*root_a)
		(*root_a)->left = NULL;
	if (flag == false)
	{
		append_astree_node(root_b, temp);
		return ;
	}
	if (!temp)
		return ;
	else if (!*root_b)
		*root_b = temp;
	else
	{
		(*root_b)->left = temp;
		temp->right = *root_b;
		*root_b = temp;
	}
}

static void	shunt_pop(t_astree **token_stack)
{
	if (!(*token_stack)->right)
	{
		free(*token_stack);
		*token_stack = NULL;
		return ;
	}
	*token_stack = (*token_stack)->right;
	free((*token_stack)->left);
	(*token_stack)->left = NULL;
}

static void	set_exec_order_p(t_astree **root, t_astree **token_stack, \
	t_astree **new)
{
	while (*root)
	{
		if ((*root)->token == NOT)
			shunt_push(root, new, false);
		else if ((*root)->token != LEFT_PAREN && (*root)->token != RIGHT_PAREN)
		{
			while (*token_stack && (*token_stack)->precedence >= (*root)->precedence)
				shunt_push(token_stack, new, false);
			shunt_push(root, token_stack, true);
		}
		else if ((*root)->token == LEFT_PAREN)
			shunt_push(root, token_stack, true);
		else if ((*root)->token == RIGHT_PAREN)
		{
			shunt_pop(root);
			while ((*token_stack)->token != LEFT_PAREN)
				shunt_push(token_stack, new, false);
			shunt_pop(token_stack);
		}
	}
}

// ? this uses the shunting yard algorithm
t_astree	*set_exec_order(t_astree **root)
{
	t_astree	*token_stack;
	t_astree	*new;

	token_stack = NULL;
	new = NULL;
	set_exec_order_p(root, &token_stack, &new);
	while (token_stack)
		shunt_push(&token_stack, &new, false);
	return (new);
}

t_astree	*balance_astree(t_astree *root)
{
	if (!root)
		return (NULL);
	if (root->token != NOT)
	{
		root->right = balance_astree(root->left);
		root->left = balance_astree(root->left);
		return (root);
	}
	if (root->left && root->right)
	{
		root->left->right = root->right;
		root->right->left = root->left;
		if (root->right->right)
			root->left->right = root->right->right;
	}
	root->right = NULL;
	root->left = NULL;
	return (root);
}

// TODO: this and all it's subfunctions should be renamed, too unclear?
t_astree	*generate_astree(char *user_input)
{
	char		*str;
	t_astree	*root;

	str = ft_strtrim(user_input, WHITESPACE);
	free(user_input);
	if (!str || !no_unexpected_start(str))
		return (free(str), NULL);
	root = generate_tree(str);
	if (!root)
		return (NULL);
	root = set_exec_order(&root);
	while (root->right)
		root = root->right;
	root = balance_astree(root);
	return (root);
}
