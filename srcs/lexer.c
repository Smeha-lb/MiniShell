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

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return;
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
	if (input[*i] == '|')
	{
		if (input[*i + 1] == '|')
		{
			add_token(&shell->tokens, create_token("||", TOKEN_OR));
			(*i) += 2;
		}
		else
		{
			add_token(&shell->tokens, create_token("|", TOKEN_PIPE));
			(*i)++;
		}
	}
	else if (input[*i] == '&' && input[*i + 1] == '&')
	{
		add_token(&shell->tokens, create_token("&&", TOKEN_AND));
		(*i) += 2;
	}
	else if (input[*i] == '<')
	{
		if (input[*i + 1] == '<')
		{
			add_token(&shell->tokens, create_token("<<", TOKEN_HEREDOC));
			(*i) += 2;
		}
		else
		{
			add_token(&shell->tokens, create_token("<", TOKEN_REDIR_IN));
			(*i)++;
		}
	}
	else if (input[*i] == '>')
	{
		if (input[*i + 1] == '>')
		{
			add_token(&shell->tokens, create_token(">>", TOKEN_REDIR_APPEND));
			(*i) += 2;
		}
		else
		{
			add_token(&shell->tokens, create_token(">", TOKEN_REDIR_OUT));
			(*i)++;
		}
	}
	else
		return (0);
	return (1);
}

int	handle_quotes(char *input, int *i, char **word, int *j, t_shell *shell)
{
	char	quote_type;
	int		start;
	char	*quoted_content;
	char	*expanded;

	quote_type = input[*i];
	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	if (!input[*i])
	{
		ft_putendl_fd("Error: Unclosed quotes", 2);
		free(*word);
		return (1);
	}
	
	// Extract the content inside quotes
	quoted_content = ft_substr(input, start, *i - start);
	
	// Expand variables if inside double quotes
	if (quote_type == '\"')
	{
		expanded = expand_variables(shell, quoted_content, 1);
		free(quoted_content);
	}
	else  // Single quotes - no expansion
	{
		expanded = quoted_content;
	}
	
	// Copy the expanded content to the word
	ft_strlcpy(*word + *j, expanded, ft_strlen(expanded) + 1);
	*j += ft_strlen(expanded);
	free(expanded);
	
	(*i)++;  // Skip the closing quote
	return (0);
}

int	extract_word(char *input, int *i, t_shell *shell)
{
	int		start;
	int		j;
	char	*word;
	char	*expanded;

	start = *i;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t' &&
		   input[*i] != '|' && input[*i] != '<' && input[*i] != '>' && 
		   !(input[*i] == '&' && input[*i + 1] == '&'))
		(*i)++;
	word = (char *)malloc((*i - start + 1) * sizeof(char));
	if (!word)
		return (1);
	j = 0;
	while (start < *i)
		word[j++] = input[start++];
	word[j] = '\0';
	
	// Expand variables in the word
	expanded = expand_variables(shell, word, 0);
	free(word);
	
	// Create token with expanded value
	add_token(&shell->tokens, create_token(expanded, TOKEN_WORD));
	free(expanded);
	return (0);
}

int	handle_word(char *input, int *i, t_shell *shell)
{
	int		j;
	char	*word;
	int		word_len;

	// First, calculate the size we need for the word
	word_len = 0;
	j = *i;
	while (input[j] && input[j] != ' ' && input[j] != '\t' &&
		   input[j] != '|' && input[j] != '<' && input[j] != '>' &&
		   !(input[j] == '&' && input[j + 1] == '&'))
	{
		if (input[j] == '\'' || input[j] == '\"')
		{
			char quote = input[j++];
			while (input[j] && input[j] != quote)
				j++;
			if (!input[j])
				break;
		}
		j++;
		word_len++;
	}
	
	// Allocate generous space for the word (larger than needed to accommodate expansions)
	word = (char *)malloc((j - *i + 1024) * sizeof(char));
	if (!word)
		return (1);
	
	// Now extract the word with proper quote handling and expansion
	j = 0;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t' &&
		   input[*i] != '|' && input[*i] != '<' && input[*i] != '>' &&
		   !(input[*i] == '&' && input[*i + 1] == '&'))
	{
		if (input[*i] == '\'' || input[*i] == '\"')
		{
			if (handle_quotes(input, i, &word, &j, shell))
				return (1);
		}
		else if (input[*i] == '$' && (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_' || input[*i + 1] == '?'))
		{
			// Handle inline variable expansion (outside quotes)
			(*i)++;
			int var_name_len = 0;
			while (input[*i + var_name_len] && (ft_isalnum(input[*i + var_name_len]) || input[*i + var_name_len] == '_' || 
				   (var_name_len == 0 && input[*i + var_name_len] == '?')))
				var_name_len++;
			
			char *var_name = ft_substr(input, *i, var_name_len);
			char *var_value;
			
			if (ft_strcmp(var_name, "?") == 0)
			{
				// Handle $? for last exit status
				char exit_status[12];
				sprintf(exit_status, "%d", shell->exit_status);
				var_value = ft_strdup(exit_status);
			}
			else
			{
				var_value = get_env_value(shell, var_name);
				if (var_value)
					var_value = ft_strdup(var_value);
				else
					var_value = ft_strdup("");
			}
			
			ft_strlcpy(word + j, var_value, ft_strlen(var_value) + 1);
			j += ft_strlen(var_value);
			
			free(var_name);
			free(var_value);
			*i += var_name_len;
		}
		else
			word[j++] = input[(*i)++];
	}
	word[j] = '\0';
	
	// Create the token with the expanded word
	add_token(&shell->tokens, create_token(word, TOKEN_WORD));
	free(word);
	return (0);
}

int	tokenize_input(t_shell *shell, char *input)
{
	int	i;

	i = 0;
	shell->tokens = NULL;
	while (input[i])
	{
		if (input[i] == ' ' || input[i] == '\t')
			i++;
		else if (handle_special_char(shell, input, &i))
			continue;
		else if (handle_word(input, &i, shell))
			return (1);
	}
	return (0);
} 