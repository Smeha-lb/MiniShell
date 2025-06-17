#include "../includes/minishell.h"

// Make previous_cmd accessible to shell_cleanup
static char *g_previous_cmd = NULL;

void	shell_init(t_shell *shell, char **env)
{
	char	*shlvl_value;
	int		shlvl;
	char	*new_shlvl;  // Change to char pointer

	shell->env = copy_env(env);
	shell->tokens = NULL;
	shell->commands = NULL;
	shell->exit_status = 0;
	shell->running = 1;

	// Increment SHLVL
	shlvl_value = get_env_value(shell, "SHLVL");
	if (shlvl_value)
		shlvl = ft_atoi(shlvl_value);
	else
		shlvl = 0;

	shlvl++;
	new_shlvl = ft_itoa(shlvl); 
	if (new_shlvl)
	{
		set_env_var(shell, "SHLVL", new_shlvl);
		free(new_shlvl);  // Free the allocated string
	}
}

void	clean_tokens(t_shell *shell)
{
	shell->exit_status = 1;
	free_tokens(shell->tokens);
	shell->tokens = NULL;
}

void	process_input(t_shell *shell, char *input)
{
	if (!input || ft_strlen(input) == 0)
		return;

	if (!g_previous_cmd || ft_strcmp(input, g_previous_cmd) != 0)
	{
		add_history(input);
		if (g_previous_cmd)
			free(g_previous_cmd);
		g_previous_cmd = ft_strdup(input);
	}
	
	if (!tokenize_input(shell, input))
		return (clean_tokens(shell));
	if (!parse_tokens(shell))
		return (clean_tokens(shell));

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
			// Always exit on EOF (Ctrl+D), regardless of previous signal
			ft_putendl_fd("exit", STDOUT_FILENO);
			break;
		}
		process_input(shell, input);
		free(input);
	}
}

void	cleanup_shlvl(t_shell *shell, int *shlvl, char *shlvl_value)
{
	char	*new_shlvl;

	if (shlvl_value)
		*shlvl = ft_atoi(shlvl_value);
	else
		*shlvl = 1;

	(*shlvl)--;
	if (*shlvl < 0) 
		*shlvl = 0;

	new_shlvl = ft_itoa(*shlvl);
	if (new_shlvl)
	{
		set_env_var(shell, "SHLVL", new_shlvl);
		free(new_shlvl);
	}
}

// Free the previous command memory
// Decrement SHLVL (but not below 0)
void	shell_cleanup(t_shell *shell)
{
	int		shlvl;
	char	*shlvl_value;
	
	if (g_previous_cmd)
	{
		free(g_previous_cmd);
		g_previous_cmd = NULL;
	}

	shlvl_value = get_env_value(shell, "SHLVL");
	cleanup_shlvl(shell, &shlvl, shlvl_value);
	if (shell->tokens)
		free_tokens(shell->tokens);
	if (shell->commands)
		free_commands(shell->commands);
	free_array(shell->env);
	rl_clear_history();
}