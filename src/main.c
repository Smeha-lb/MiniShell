/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:12 by moabdels          #+#    #+#             */
/*   Updated: 2025/05/12 17:43:02 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

t_signal_handler	signal_handler;

static pid_t	get_pid(void)
{
	DIR				*dir;
	struct dirent	*file;
	pid_t			pid;

	pid = 0;
	dir = opendir("/proc/self/task");
	if (dir == NULL)
		return (pid);
	file = readdir(dir);
	while (file != NULL && pid == 0)
	{
		pid = ft_atoi(file->d_name);
		file = readdir(dir);
	}
	closedir(dir);
	return (pid);
}

static void	shell_init(t_minishell *minishell)
{
	minishell->is_tty = isatty(STDIN_FILENO);
	minishell->shell_pid = get_pid();
	minishell->subshell_pid = -1;
	minishell->exit_status = OK;
	// TODO: Signal handling.
	// TODO: Clean on exit: Clear lists, free cmd table, clear history
}

// line is a pointer to a string, not a 2d array
// we love the C type system don't we folks?

// ? we're saying here that we don't care about interrupts,
// ?  but we do care about other errors.
// * `return (free(temp), false)` -> this works because:
// * 1 - return takes any expression
// * 2 - comma is an operator that takes any number of expressions, evaluates
// * them, then discards everything but the rightmost expression.

static bool	get_input(char **line, bool update_hist, bool is_tty)
{
	char	*temp;

	errno = OK;
	*line = NULL;  // Initialize line to NULL
	if (!is_tty)  // Changed from if (is_tty) to if (!is_tty)
		*line = readline("minishell%%> ");
	else
	{
		temp = get_next_line(STDIN_FILENO);
		if (errno != OK)
			return (free(temp), false);
		if (temp)
		{
			*line = ft_strtrim(temp, "\n");
			free(temp);
		}
	}
	if (errno == EINTR)
		errno = OK;
	else if (errno != OK)
		return (false);
	if (update_hist && *line && **line)
		add_history(*line);
	return (errno == OK);
}

static void	print_ast_node(t_astree *node, int depth)
{
	int	i;

	if (!node)
		return;
	for (i = 0; i < depth; i++)
		printf("  ");
	printf("Node at depth %d:\n", depth);
	for (i = 0; i < depth; i++)
		printf("  ");
	printf("Command: %s\n", node->cmd ? node->cmd[0] : "NULL");
	for (i = 0; i < depth; i++)
		printf("  ");
	printf("Token: %d\n", node->token);
	for (i = 0; i < depth; i++)
		printf("  ");
	printf("Precedence: %d\n", node->precedence);
	for (i = 0; i < depth; i++)
		printf("  ");
	printf("FD in: %d, FD out: %d\n", node->fd_in, node->fd_out);
	if (node->redir_tree)
	{
		for (i = 0; i < depth; i++)
			printf("  ");
		printf("Has redirects\n");
	}
	printf("\n");
	print_ast_node(node->left, depth + 1);
	print_ast_node(node->right, depth + 1);
}

void	print_astree_detail(t_astree *root)
{
	ft_printf(MSH_DEBUG"AST Structure:\n");
	print_ast_node(root, 0);
}

// TODO: create a function to turn env to a linked list
// ! should main take envp?
int	main(void)
{
	t_minishell	minishell;
	t_astree	*ast;

	minishell.user_input = NULL;
	printf("Starting minishell...\n");  // Debug print
	shell_init(&minishell);
	while (1)
	{
		printf("Waiting for input...\n");  // Debug print
		if (!get_input(&minishell.user_input, true, minishell.is_tty))
		{
			printf("get_input failed\n");  // Debug print
			continue;
		}
		if (!minishell.user_input)
		{
			printf("exit\n");
			break;
		}
		printf("Got input: %s\n", minishell.user_input);  // Debug print
		ast = generate_astree(minishell.user_input);
		printf("Got to this line\n");  // Debug print
		if (ast)
		{
			printf("\nAST Structure:\n");
			printf("------------------------\n");
			print_astree_detail(ast);
			printf("------------------------\n");
			// TODO: Add proper AST cleanup function
			// For now, we'll just free the AST structure
			free(ast);
		}
		else
		{
			printf("Failed to generate AST\n");  // Debug print
		}
		// Don't free line here as it's already freed by generate_astree
	}
	return (0);
}
