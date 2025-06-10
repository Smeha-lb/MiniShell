#include "../includes/minishell.h"

void	initialize_minimal_env(t_shell *shell)
{
	char	cwd[1024];
	char	*empty_env[1];

	empty_env[0] = NULL;
	shell->env = copy_env(empty_env);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		set_env_var(shell, "PWD", cwd);
	else
		set_env_var(shell, "PWD", "/");
	set_env_var(shell, "SHLVL", "2");
	set_env_var(shell, "PATH", "/usr/local/bin:/usr/bin:/bin");
	set_env_var(shell, "_", "./minishell");
}

void	process_input(t_shell *shell, char *input)
{
	if (!input || ft_strlen(input) == 0)
		return ;
	if (!shell->previous_cmd || ft_strcmp(input, shell->previous_cmd) != 0)
	{
		add_history(input);
		if (shell->previous_cmd)
			free(shell->previous_cmd);
		shell->previous_cmd = ft_strdup(input);
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
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
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

void	shell_cleanup(t_shell *shell)
{
	int		shlvl;
	char	*shlvl_value;

	if (shell->previous_cmd)
	{
		free(shell->previous_cmd);
		shell->previous_cmd = NULL;
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
