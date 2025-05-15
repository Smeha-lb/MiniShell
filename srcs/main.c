/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	g_exit_status;

/**
 * Initialize the minishell structure
 */
static t_minishell	*init_shell(char **envp)
{
	t_minishell	*shell;

	shell = (t_minishell *)malloc(sizeof(t_minishell));
	if (!shell)
		return (NULL);
	shell->env = init_env(envp);
	if (!shell->env)
	{
		free(shell);
		return (NULL);
	}
	shell->ast = NULL;
	shell->last_exit_status = 0;
	shell->running = 1;
	shell->in_heredoc = 0;
	g_exit_status = 0;
	return (shell);
}

/**
 * Clean up and free resources before exiting
 */
static void	cleanup_shell(t_minishell *shell)
{
	if (shell)
	{
		if (shell->env)
			free_array(shell->env);
		if (shell->ast)
			free_ast(shell->ast);
		free(shell);
	}
}

/**
 * Get or set the previous command
 * If cmd is NULL, returns current value
 * If cmd is not NULL, updates the value and returns it
 * If action is 1, frees the memory and returns NULL
 */
static char	*prev_cmd_handler(char *cmd, int action)
{
	static char	*prev_cmd = NULL;

	if (action == 1)
	{
		free(prev_cmd);
		prev_cmd = NULL;
		return (NULL);
	}

	if (cmd)
	{
		free(prev_cmd);
		prev_cmd = ft_strdup(cmd);
	}
	
	return (prev_cmd);
}

/**
 * Process a single input line
 */
static int	process_line(t_minishell *shell, char *line)
{
	t_token	*tokens;
	char	*prev_cmd;

	if (!line || !*line)
		return (0);
		
	// Only add to history if different from previous command
	prev_cmd = prev_cmd_handler(NULL, 0);
	if (!prev_cmd || ft_strcmp(line, prev_cmd) != 0)
		add_history(line);
		
	// Update previous command
	prev_cmd_handler(line, 0);
	
	tokens = tokenize(line);
	if (!tokens)
	{
		free(line);
		return (0);
	}
	shell->ast = parse(tokens);
	free_tokens(tokens);
	if (!shell->ast)
	{
		free(line);
		return (0);
	}
	shell->last_exit_status = execute_ast(shell, shell->ast);
	g_exit_status = shell->last_exit_status;
	free_ast(shell->ast);
	shell->ast = NULL;
	free(line);
	return (1);
}

/**
 * Main shell loop
 */
static void	shell_loop(t_minishell *shell)
{
	char	*line;

	setup_signals(0);
	while (shell->running)
	{
		line = readline(PROMPT);
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		process_line(shell, line);
	}
	
	// Cleanup prev_cmd when exiting
	prev_cmd_handler(NULL, 1);
}

/**
 * Main function
 */
int	main(int argc, char **argv, char **envp)
{
	t_minishell	*shell;

	(void)argc;
	(void)argv;
	shell = init_shell(envp);
	if (!shell)
	{
		print_error("init", "failed to initialize shell");
		return (1);
	}
	shell_loop(shell);
	cleanup_shell(shell);
	return (0);
} 