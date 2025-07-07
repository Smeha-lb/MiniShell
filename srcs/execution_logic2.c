#include "../includes/minishell.h"

// Forward declaration
void	split_expanded_cmd_name(t_shell *shell, t_command *cmd);

// Split command name with variables
void	split_expanded_cmd_name(t_shell *shell, t_command *cmd)
{
	char	*expanded;
	char	**split_args;
	int		i;
	int		j;
	char	**new_args;
	int		*new_quoted;

	if (!cmd->args || !cmd->args[0])
		return;
	
	expanded = expand_token(shell, cmd->args[0]);
	if (!expanded || !ft_strchr(expanded, ' '))
	{
		free(expanded);
		return;
	}
	
	split_args = ft_split(expanded, ' ');
	free(expanded);
	
	if (!split_args || !split_args[0])
	{
		free_array(split_args);
		return;
	}
	
	// Count original args
	i = 0;
	while (cmd->args[i])
		i++;
	
	// Count split args
	j = 0;
	while (split_args[j])
		j++;
	
	// Allocate new args array with space for all
	new_args = (char **)malloc(sizeof(char *) * (i + j));
	new_quoted = (int *)malloc(sizeof(int) * (i + j));
	if (!new_args || !new_quoted)
	{
		free_array(split_args);
		free(new_args);
		free(new_quoted);
		return;
	}
	
	// Copy split command name parts
	for (j = 0; split_args[j]; j++)
	{
		new_args[j] = split_args[j];
		new_quoted[j] = 0;
	}
	
	// Copy the rest of original args
	for (i = 1; cmd->args[i]; i++, j++)
	{
		new_args[j] = ft_strdup(cmd->args[i]);
		if (cmd->arg_quoted)
			new_quoted[j] = cmd->arg_quoted[i];
		else
			new_quoted[j] = 0;
	}
	new_args[j] = NULL;
	
	// Free old args and update
	free_cmd_args(cmd->args);
	free(cmd->arg_quoted);
	free(split_args); // Just free the array, not the contents
	
	cmd->args = new_args;
	cmd->arg_quoted = new_quoted;
}

// Set the quoted flag for a new argument
void	set_quoted_flag(int *new_quoted, int k, t_command *cmd, int i)
{
	if (cmd->arg_quoted)
		new_quoted[k] = cmd->arg_quoted[i];
	else
		new_quoted[k] = 0;
}

// Handle expanded arguments
void	handle_expanded_args(t_shell *shell, t_command *cmd, char ***new_args,
		int **new_quoted)
{
	char	*expanded;
	char	**split_args;
	int		i;
	int		j;
	int		k;
	int		should_split;

	// Special handling for command name with variables
	if (cmd->args[0] && ft_strchr(cmd->args[0], '$'))
	{
		split_expanded_cmd_name(shell, cmd);
		// If the command was split, we're done
		if (cmd->args && cmd->args[0] && !ft_strchr(cmd->args[0], '$'))
		{
			// Copy the args to new_args
			i = 0;
			while (cmd->args[i])
			{
				(*new_args)[i] = ft_strdup(cmd->args[i]);
				if (cmd->arg_quoted)
					(*new_quoted)[i] = cmd->arg_quoted[i];
				else
					(*new_quoted)[i] = 0;
				i++;
			}
			(*new_args)[i] = NULL;
			return;
		}
	}

	i = 0;
	k = 0;
	while (cmd->args[i])
	{
		// Don't split arguments that are quoted
		should_split = should_split_arg(cmd->args[i]) && (!cmd->arg_quoted || !cmd->arg_quoted[i]);
		
		if (cmd->arg_quoted && cmd->arg_quoted[i])
		{
			// For quoted arguments, just expand but don't split
			expanded = expand_token(shell, cmd->args[i]);
			if (!expanded)
				expanded = ft_strdup(cmd->args[i]);
			
			(*new_args)[k] = expanded;
			(*new_quoted)[k] = cmd->arg_quoted[i];
			k++;
		}
		else if (!should_split)
		{
			expanded = expand_token(shell, cmd->args[i]);
			if (!expanded)
			{
				(*new_args)[k] = ft_strdup(cmd->args[i]);
				set_quoted_flag(*new_quoted, k, cmd, i);
				k++;
			}
			else
			{
				(*new_args)[k] = expanded;
				set_quoted_flag(*new_quoted, k, cmd, i);
				k++;
			}
		}
		else
		{
			expanded = expand_token(shell, cmd->args[i]);
			if (!expanded)
			{
				(*new_args)[k] = ft_strdup(cmd->args[i]);
				set_quoted_flag(*new_quoted, k, cmd, i);
				k++;
			}
			else
			{
				split_args = ft_split(expanded, ' ');
				if (split_args)
				{
					j = 0;
					while (split_args[j])
					{
						(*new_args)[k] = split_args[j];
						(*new_quoted)[k] = 0;
						k++;
						j++;
					}
					free(split_args);
				}
				free(expanded);
			}
		}
		i++;
	}
	(*new_args)[k] = NULL;
}

// Split expanded command if needed - improved version
void	split_command_if_needed(t_shell *shell, t_command *cmd)
{
	int		i;
	char	**new_args;
	int		*new_quoted;

	if (!cmd->args || !cmd->args[0])
		return;
	i = 0;
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc(sizeof(char *) * (i * 10 + 1));
	new_quoted = (int *)malloc(sizeof(int) * (i * 10 + 1));
	if (!new_args || !new_quoted)
	{
		free(new_args);
		free(new_quoted);
		return;
	}
	handle_expanded_args(shell, cmd, &new_args, &new_quoted);
	free_cmd_args(cmd->args);
	free(cmd->arg_quoted);
	cmd->args = new_args;
	cmd->arg_quoted = new_quoted;
}