#include "../includes/minishell.h"

static void	copy_env_vars(char **env, char **sorted_env, int env_size)
{
	int	i;

	i = 0;
	while (i < env_size)
	{
		sorted_env[i] = ft_strdup(env[i]);
		i++;
	}
	sorted_env[i] = NULL;
}

/**
 * Allocate and prepare sorted environment array
 */
char	**prepare_sorted_env(t_shell *shell, int *env_size)
{
	char	**sorted_env;

	*env_size = count_array(shell->env);
	sorted_env = (char **)malloc((*env_size + 1) * sizeof(char *));
	if (!sorted_env)
		return (NULL);
	copy_env_vars(shell->env, sorted_env, *env_size);
	sort_env_vars(sorted_env, *env_size);
	return (sorted_env);
}

/**
 * Display all environment variables and cleanup
 */
void	display_and_cleanup_env(char **sorted_env, int env_size)
{
	int		i;

	i = 0;
	while (i < env_size)
	{
		display_env_var(sorted_env[i]);
		i++;
	}
	i = 0;
	while (i < env_size)
	{
		free(sorted_env[i]);
		i++;
	}
	free(sorted_env);
}

/**
 * Process a single export argument
 */
int	process_single_export_arg(t_shell *shell, char *arg)
{
	char	*error;

	error = is_valid_export_arg(arg);
	if (error)
	{
		ft_putstr_fd("export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': ", STDERR_FILENO);
		ft_putendl_fd(error, STDERR_FILENO);
		return (1);
	}
	else
	{
		process_export_arg(shell, arg);
		return (0);
	}
}

/**
 * Process all export arguments
 */
int	process_export_args(t_shell *shell, t_command *cmd)
{
	int		i;
	int		status;

	status = 0;
	i = 1;
	while (cmd->args[i])
	{
		if (process_single_export_arg(shell, cmd->args[i]) != 0)
			status = 1;
		i++;
	}
	return (status);
}
