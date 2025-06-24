#include "../includes/minishell.h"

t_token	*create_token(char *value, t_token_type type)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->value = ft_strdup(value);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->type = type;
	token->next = NULL;
	return (token);
}

void	add_to_token_list(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	free_tokens(t_token *tokens)
{
	t_token	*temp;

	while (tokens)
	{
		temp = tokens;
		tokens = tokens->next;
		free(temp->value);
		free(temp);
	}
}

int	handle_special_char(t_shell *shell, char *input, int *i)
{
	if (input[*i] == '(')
	{
		add_to_token_list(&shell->tokens, create_token("(", TOKEN_LPAREN));
		(*i)++;
	}
	else if (input[*i] == ')')
	{
		add_to_token_list(&shell->tokens, create_token(")", TOKEN_RPAREN));
		(*i)++;
	}
	else if (input[*i] == '|')
	{
		if (input[*i + 1] == '|')
		{
			add_to_token_list(&shell->tokens, create_token("||", TOKEN_OR));
			(*i) += 2;
		}
		else
		{
			add_to_token_list(&shell->tokens, create_token("|", TOKEN_PIPE));
			(*i)++;
		}
	}
	else if (input[*i] == '&' && input[*i + 1] == '&')
	{
		add_to_token_list(&shell->tokens, create_token("&&", TOKEN_AND));
		(*i) += 2;
	}
	else if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			add_to_token_list(&shell->tokens,
				create_token("<<", TOKEN_HEREDOC));
			(*i) += 2;
		}
		else
		{
			add_to_token_list(&shell->tokens,
				create_token("<", TOKEN_REDIR_IN));
			(*i)++;
		}
	}
	else if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			add_to_token_list(&shell->tokens,
				create_token(">>", TOKEN_REDIR_APPEND));
			(*i) += 2;
		}
		else
		{
			add_to_token_list(&shell->tokens,
				create_token(">", TOKEN_REDIR_OUT));
			(*i)++;
		}
	}
	else
		return (0);
	return (1);
}
