#include "../includes/minishell.h"

t_token	*find_matching_paren(t_token *start)
{
	int		paren_count;
	t_token	*token;

	paren_count = 1;
	token = start->next;
	if (!token || start->type != TOKEN_LPAREN)
		return (NULL);
	while (token && paren_count > 0)
	{
		if (token->type == TOKEN_LPAREN)
			paren_count++;
		else if (token->type == TOKEN_RPAREN)
			paren_count--;
		if (paren_count == 0)
			return (token);
		token = token->next;
	}
	return (NULL);
}

t_token	*copy_tokens_section(t_token *start, t_token *end)
{
	t_token	*head;
	t_token	*token;
	t_token	*new_token;

	head = NULL;
	token = start->next;
	if (!token || token == end)
		return (NULL);
	while (token && token != end)
	{
		new_token = create_token(token->value, token->type, token->quoted);
		if (!new_token)
		{
			if (head)
				free_tokens(head);
			return (NULL);
		}
		add_to_token_list(&head, new_token);
		token = token->next;
	}
	return (head);
}

t_command	*parse_subshell(t_token *start, t_token *end)
{
	t_shell		temp_shell;
	t_token		*subshell_tokens;

	temp_shell.tokens = NULL;
	temp_shell.commands = NULL;
	temp_shell.exit_status = 0;
	temp_shell.running = 1;
	temp_shell.env = NULL;
	subshell_tokens = copy_tokens_section(start, end);
	if (!subshell_tokens && start->next != end)
	{
		ft_putendl_fd("Error: Failed to parse subshell", 2);
		return (NULL);
	}
	temp_shell.tokens = subshell_tokens;
	if (!parse_tokens(&temp_shell))
	{
		if (subshell_tokens)
			free_tokens(subshell_tokens);
		return (NULL);
	}
	if (subshell_tokens)
		free_tokens(subshell_tokens);
	return (temp_shell.commands);
}
