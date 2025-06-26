#include "../includes/minishell.h"

int	handle_and(t_shell *shell, char *input, int *i)
{
	if (input[*i] == '&' && input[*i + 1] == '&')
	{
		add_to_token_list(&shell->tokens, create_token("&&", TOKEN_AND, 0));
		(*i) += 2;
		return (1);
	}
	return (0);
}

int	handle_input_redirection(t_shell *shell, char *input, int *i)
{
	if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			add_to_token_list(&shell->tokens,
				create_token("<<", TOKEN_HEREDOC, 0));
			(*i) += 2;
		}
		else
		{
			add_to_token_list(&shell->tokens,
				create_token("<", TOKEN_REDIR_IN, 0));
			(*i)++;
		}
		return (1);
	}
	return (0);
}

int	handle_output_redirection(t_shell *shell, char *input, int *i)
{
	if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			add_to_token_list(&shell->tokens,
				create_token(">>", TOKEN_REDIR_APPEND, 0));
			(*i) += 2;
		}
		else
		{
			add_to_token_list(&shell->tokens,
				create_token(">", TOKEN_REDIR_OUT, 0));
			(*i)++;
		}
		return (1);
	}
	return (0);
}

int	handle_redirections(t_shell *shell, char *input, int *i)
{
	if (handle_input_redirection(shell, input, i))
		return (1);
	if (handle_output_redirection(shell, input, i))
		return (1);
	return (0);
}

int	handle_special_char(t_shell *shell, char *input, int *i)
{
	if (handle_parentheses(shell, input, i))
		return (1);
	if (handle_pipe_or(shell, input, i))
		return (1);
	if (handle_and(shell, input, i))
		return (1);
	if (handle_redirections(shell, input, i))
		return (1);
	return (0);
}
