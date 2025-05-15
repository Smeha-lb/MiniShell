/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/01 00:00:00                      #+#    #+#             */
/*   Updated: 2023/01/01 00:00:00                     ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

extern int	g_exit_status;

/**
 * Process arguments by expanding variables and removing quotes
 */
static char	**process_args(t_minishell *shell, char **args)
{
	char	**processed;
	int		i;
	int		len;
	char	*expanded;

	if (!args)
		return (NULL);
	len = 0;
	while (args[len])
		len++;
	processed = (char **)malloc(sizeof(char *) * (len + 1));
	if (!processed)
		return (NULL);
	i = 0;
	while (i < len)
	{
		expanded = expand_variables(shell, args[i]);
		processed[i] = remove_quotes(expanded);
		free(expanded);
		i++;
	}
	processed[i] = NULL;
	return (processed);
}

/**
 * Execute a builtin command
 */
static int	execute_builtin(t_minishell *shell, t_command *cmd)
{
	char	**processed_args;
	int		ret;

	if (!cmd->args || !cmd->args[0])
		return (0);
	processed_args = process_args(shell, cmd->args);
	if (!processed_args)
		return (1);
	if (ft_strcmp(processed_args[0], "echo") == 0)
		ret = ft_echo(processed_args);
	else if (ft_strcmp(processed_args[0], "cd") == 0)
		ret = ft_cd(shell, processed_args);
	else if (ft_strcmp(processed_args[0], "pwd") == 0)
		ret = ft_pwd();
	else if (ft_strcmp(processed_args[0], "export") == 0)
		ret = ft_export(shell, processed_args);
	else if (ft_strcmp(processed_args[0], "unset") == 0)
		ret = ft_unset(shell, processed_args);
	else if (ft_strcmp(processed_args[0], "env") == 0)
		ret = ft_env(shell);
	else if (ft_strcmp(processed_args[0], "exit") == 0)
		ret = ft_exit(shell, processed_args);
	else
		ret = -1;
	free_array(processed_args);
	return (ret);
}

/**
 * Find a command in the PATH
 */
static char	*find_command(char **env, char *cmd, t_minishell *shell)
{
	char	*path;
	char	*dir;
	char	*full_path;
	char	**paths;
	int		i;
	char	*expanded;
	char	*unquoted_cmd;

	expanded = expand_variables(shell, cmd);
	unquoted_cmd = remove_quotes(expanded);
	free(expanded);
	if (ft_strchr(unquoted_cmd, '/'))
	{
		char *result = ft_strdup(unquoted_cmd);
		free(unquoted_cmd);
		return (result);
	}
	path = get_env_value(env, "PATH");
	if (!path)
	{
		free(unquoted_cmd);
		return (NULL);
	}
	paths = ft_split(path, ':');
	if (!paths)
	{
		free(unquoted_cmd);
		return (NULL);
	}
	i = 0;
	while (paths[i])
	{
		dir = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(dir, unquoted_cmd);
		free(dir);
		if (access(full_path, X_OK) == 0)
		{
			free(unquoted_cmd);
			free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free(unquoted_cmd);
	free_array(paths);
	return (NULL);
}

/**
 * Execute an external command
 */
static int	execute_external(t_minishell *shell, t_command *cmd)
{
	char	*cmd_path;
	pid_t	pid;
	int		status;
	char	**processed_args;

	if (!cmd->args || !cmd->args[0])
		return (0);
	processed_args = process_args(shell, cmd->args);
	if (!processed_args)
		return (1);
	cmd_path = find_command(shell->env, cmd->args[0], shell);
	if (!cmd_path)
	{
		print_error(processed_args[0], "command not found");
		free_array(processed_args);
		g_exit_status = 127;
		return (127);
	}
	pid = fork();
	if (pid < 0)
	{
		print_error("fork", NULL);
		free(cmd_path);
		free_array(processed_args);
		g_exit_status = 1;
		return (1);
	}
	else if (pid == 0)
	{
		setup_redirections(cmd->redirections, shell);
		execve(cmd_path, processed_args, shell->env);
		print_error(processed_args[0], NULL);
		exit(126);
	}
	else
	{
		free(cmd_path);
		free_array(processed_args);
		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
		{
			g_exit_status = WEXITSTATUS(status);
			return (WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status))
		{
			g_exit_status = 128 + WTERMSIG(status);
			return (128 + WTERMSIG(status));
		}
	}
	return (0);
}

/**
 * Setup redirections for a command
 */
int	setup_redirections(t_redir *redirs, t_minishell *shell)
{
	int		fd;
	t_redir	*current;
	char	*expanded;
	char	*unquoted_file;

	current = redirs;
	while (current)
	{
		expanded = expand_variables(shell, current->file);
		unquoted_file = remove_quotes(expanded);
		free(expanded);

		if (current->type == REDIR_IN)
		{
			fd = open(unquoted_file, O_RDONLY);
			if (fd < 0)
			{
				print_error(unquoted_file, NULL);
				free(unquoted_file);
				g_exit_status = 1;
				return (0);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (current->type == REDIR_OUT)
		{
			fd = open(unquoted_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd < 0)
			{
				print_error(unquoted_file, NULL);
				free(unquoted_file);
				g_exit_status = 1;
				return (0);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (current->type == REDIR_APPEND)
		{
			fd = open(unquoted_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd < 0)
			{
				print_error(unquoted_file, NULL);
				free(unquoted_file);
				g_exit_status = 1;
				return (0);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		free(unquoted_file);
		current = current->next;
	}
	return (1);
}

/**
 * Execute a single command
 */
int	execute_command(t_minishell *shell, t_command *cmd)
{
	int	ret;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (0);
	ret = execute_builtin(shell, cmd);
	if (ret >= 0)
	{
		g_exit_status = ret;
		return (ret);
	}
	return (execute_external(shell, cmd));
}

/**
 * Execute the AST
 */
int	execute_ast(t_minishell *shell, t_pipeline *ast)
{
	int	exit_status;

	exit_status = 0;
	while (ast)
	{
		exit_status = execute_pipeline(shell, ast->commands);
		ast = ast->next;
	}
	g_exit_status = exit_status;
	return (exit_status);
} 