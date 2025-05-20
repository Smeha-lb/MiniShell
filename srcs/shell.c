#include "../includes/minishell.h"

void	shell_init(t_shell *shell, char **env)
{
	shell->env = copy_env(env);
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->exit_status = 0;
	shell->running = 1;
}

void	process_input(t_shell *shell, char *input)
{
	if (!input || ft_strlen(input) == 0)
		return;
	
	add_history(input);
	
	if (tokenize_input(shell, input) != 0)
	{
		shell->exit_status = 1;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return;
	}
	
	if (parse_tokens(shell) != 0)
	{
		shell->exit_status = 1;
		free_tokens(shell->tokens);
		shell->tokens = NULL;
		return;
	}
	
	shell->exit_status = execute_commands(shell);
	
	free_tokens(shell->tokens);
	free_commands(shell->commands);
	shell->tokens = NULL;
	shell->commands = NULL;
}

void	shell_loop(t_shell *shell)
{
	char	*input;

	while (shell->running)
	{
		g_signal_code = 0;
		input = readline(PROMPT);
		
		if (!input)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break;
		}
		
		process_input(shell, input);
		free(input);
	}
}

void	shell_cleanup(t_shell *shell)
{
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	free_array(shell->env);
	rl_clear_history();
} 