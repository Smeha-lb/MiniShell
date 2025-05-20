#include "../includes/minishell.h"

t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = (t_command *)malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirs = NULL;
	cmd->pipe_in = -1;
	cmd->pipe_out = -1;
	cmd->next_op = 0;  // Initialize next_op to 0 (none)
	cmd->next = NULL;
	return (cmd);
}

void	add_redir(t_redir **head, int type, char *file)
{
	t_redir	*new_redir;
	t_redir	*current;

	new_redir = (t_redir *)malloc(sizeof(t_redir));
	if (!new_redir)
		return;
	new_redir->type = type;
	new_redir->file = ft_strdup(file);
	new_redir->next = NULL;
	if (!*head)
	{
		*head = new_redir;
		return;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

void	add_arg(t_command *cmd, char *arg)
{
	int		i;
	char	**new_args;

	if (!cmd->args)
	{
		cmd->args = (char **)malloc(2 * sizeof(char *));
		if (!cmd->args)
			return;
		cmd->args[0] = ft_strdup(arg);
		cmd->args[1] = NULL;
		return;
	}
	i = 0;
	while (cmd->args[i])
		i++;
	new_args = (char **)malloc((i + 2) * sizeof(char *));
	if (!new_args)
		return;
	i = 0;
	while (cmd->args[i])
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = ft_strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

void	free_redirs(t_redir *redirs)
{
	t_redir	*temp;

	while (redirs)
	{
		temp = redirs;
		redirs = redirs->next;
		free(temp->file);
		free(temp);
	}
}

void	free_commands(t_command *commands)
{
	t_command	*temp;
	int			i;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		if (temp->args)
		{
			i = 0;
			while (temp->args[i])
			{
				free(temp->args[i]);
				i++;
			}
			free(temp->args);
		}
		free_redirs(temp->redirs);
		free(temp);
	}
}

t_token	*handle_redir(t_token *token, t_command *cmd)
{
	t_token_type	type;
	t_token			*next_token;

	type = token->type;
	next_token = token->next;
	if (!next_token || next_token->type != TOKEN_WORD)
	{
		ft_putendl_fd("Error: Syntax error near redirection", 2);
		return (NULL);
	}
	if (type == TOKEN_REDIR_IN)
		add_redir(&cmd->redirs, TOKEN_REDIR_IN, next_token->value);
	else if (type == TOKEN_REDIR_OUT)
		add_redir(&cmd->redirs, TOKEN_REDIR_OUT, next_token->value);
	else if (type == TOKEN_REDIR_APPEND)
		add_redir(&cmd->redirs, TOKEN_REDIR_APPEND, next_token->value);
	else if (type == TOKEN_HEREDOC)
		add_redir(&cmd->redirs, TOKEN_HEREDOC, next_token->value);
	return (next_token);
}

int	parse_tokens(t_shell *shell)
{
	t_token		*token;
	t_command	*cmd;
	t_command	*cmd_head;

	token = shell->tokens;
	if (!token)
		return (0);
	cmd = create_command();
	cmd_head = cmd;
	while (token)
	{
		if (token->type == TOKEN_WORD)
			add_arg(cmd, token->value);
		else if (token->type == TOKEN_PIPE)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `|'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_AND)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `&&'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next_op = 1;  // Mark for AND operator
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_OR)
		{
			if (!token->next)
			{
				ft_putendl_fd("Error: Syntax error near unexpected token `||'", 2);
				free_commands(cmd_head);
				return (1);
			}
			cmd->next_op = 2;  // Mark for OR operator
			cmd->next = create_command();
			cmd = cmd->next;
		}
		else if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT ||
				token->type == TOKEN_REDIR_APPEND || token->type == TOKEN_HEREDOC)
		{
			token = handle_redir(token, cmd);
			if (!token)
			{
				free_commands(cmd_head);
				return (1);
			}
		}
		token = token->next;
	}
	shell->commands = cmd_head;
	return (0);
} 