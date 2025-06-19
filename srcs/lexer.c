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
	if (input[*i] == '(')
	{
		add_token(&shell->tokens, create_token("(", TOKEN_LPAREN));
		(*i)++;
	}
	else if (input[*i] == ')')
	{
		add_token(&shell->tokens, create_token(")", TOKEN_RPAREN));
		(*i)++;
	}
	else if (input[*i] == '|')
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
	size_t	content_len;

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
	if (!quoted_content)
	{
		free(*word);
		return (1);
	}
	
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
	
	if (!expanded)
	{
		free(*word);
		return (1);
	}
	
	// Get the expanded content length
	content_len = ft_strlen(expanded);
	
	// Copy the expanded content to the word
	ft_strlcpy(*word + *j, expanded, content_len + 1);
	*j += content_len;
	free(expanded);
	
	(*i)++;  // Skip the closing quote
	return (0);
}

// Function to split expanded variable value and add multiple tokens if needed
// This should only be used for simple variable expansions (like $VAR as a standalone word)
void	add_expanded_tokens(t_shell *shell, char *expanded_value)
{
	char	**split_values;
	int		i;

	// Check if the expanded value contains spaces
	if (ft_strchr(expanded_value, ' '))
	{
		// Split the expanded value into multiple tokens
		split_values = ft_split(expanded_value, ' ');
		if (split_values)
		{
			i = 0;
			while (split_values[i])
			{
				// Skip empty tokens
				if (ft_strlen(split_values[i]) > 0)
					add_token(&shell->tokens, create_token(split_values[i], TOKEN_WORD));
				i++;
			}
			free_array(split_values);
		}
	}
	else
	{
		// Single token, add it directly (if not empty)
		if (ft_strlen(expanded_value) > 0)
			add_token(&shell->tokens, create_token(expanded_value, TOKEN_WORD));
	}
}

int	handle_complex_word(char *input, int *i, t_shell *shell);

int	extract_word(char *input, int *i, t_shell *shell)
{
	int		start;
	int		j;
	char	*word;
	char	*expanded;

	start = *i;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t' &&
		   input[*i] != '|' && input[*i] != '<' && input[*i] != '>' && 
		   input[*i] != '(' && input[*i] != ')' &&
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
	
	// Create token(s) with expanded value, splitting on spaces if necessary
	add_expanded_tokens(shell, expanded);
	free(expanded);
	return (0);
}

int	handle_word(char *input, int *i, t_shell *shell)
{
	int		start_pos = *i;
	char	*var_name;
	char	*var_value;
	int		var_name_len;
	
	// Check if this is a simple variable expansion (like $VAR)
	if (input[*i] == '$' && (ft_isalnum(input[*i + 1]) || input[*i + 1] == '_' || input[*i + 1] == '?'))
	{
		(*i)++; // Skip $
		var_name_len = 0;
		while (input[*i + var_name_len] && (ft_isalnum(input[*i + var_name_len]) || input[*i + var_name_len] == '_' || 
			   (var_name_len == 0 && input[*i + var_name_len] == '?')))
			var_name_len++;
		
		// Check if this is the entire word (no additional characters)
		int temp_i = *i + var_name_len;
		if (input[temp_i] == '\0' || input[temp_i] == ' ' || input[temp_i] == '\t' ||
			input[temp_i] == '|' || input[temp_i] == '<' || input[temp_i] == '>' ||
			input[temp_i] == '(' || input[temp_i] == ')' ||
			(input[temp_i] == '&' && input[temp_i + 1] == '&'))
		{
			// This is a simple variable expansion, handle it with splitting
			var_name = ft_substr(input, *i, var_name_len);
			if (!var_name)
				return (1);
			
			if (ft_strcmp(var_name, "?") == 0)
			{
				char *exit_status = ft_itoa(shell->exit_status);
				if (!exit_status)
				{
					free(var_name);
					return (1);
				}
				var_value = exit_status;
			}
			else
			{
				var_value = get_env_value(shell, var_name);
				if (var_value)
					var_value = ft_strdup(var_value);
				else
					var_value = ft_strdup("");
			}
			
			if (!var_value)
			{
				free(var_name);
				return (1);
			}
			
			*i += var_name_len;
			
			// Use add_expanded_tokens to handle splitting
			add_expanded_tokens(shell, var_value);
			
			free(var_name);
			free(var_value);
			return (0);
		}
		else
		{
			// Reset position and fall back to complex word handling
			*i = start_pos;
		}
	}
	
	// Fall back to the original complex word handling logic
	return (handle_complex_word(input, i, shell));
}

int	handle_complex_word(char *input, int *i, t_shell *shell)
{
	int		j;
	char	*word;
	int		buffer_size;
	int		var_name_len;
	char	*var_name;
	char	*var_value;
	size_t	value_len;

	// Calculate a safe buffer size - count all characters including potential variable expansions
	buffer_size = 0;
	j = *i;
	while (input[j] && input[j] != ' ' && input[j] != '\t' &&
		   input[j] != '|' && input[j] != '<' && input[j] != '>' &&
		   input[j] != '(' && input[j] != ')' &&
		   !(input[j] == '&' && input[j + 1] == '&'))
	{
		// For quoted sections, count the entire quoted content
		if (input[j] == '\'' || input[j] == '\"')
		{
			char quote = input[j++];
			int quote_start = j;
			
			while (input[j] && input[j] != quote)
				j++;
				
			if (!input[j])  // Unclosed quote
				break;
				
			// For double quotes, variables can expand, so add extra space
			if (quote == '\"')
				buffer_size += (j - quote_start) * 2;  // Conservative estimate
			else
				buffer_size += (j - quote_start);
				
			j++;  // Skip closing quote
		}
		// For variables, estimate expansion size
		else if (input[j] == '$' && (ft_isalnum(input[j + 1]) || input[j + 1] == '_' || input[j + 1] == '?'))
		{
			j++;  // Skip $
			int var_start = j;
			
			while (input[j] && (ft_isalnum(input[j]) || input[j] == '_' || 
				  (j == var_start && input[j] == '?')))
				j++;
				
			// Add extra space for potentially long var expansion
			buffer_size += 128;  // Conservative estimate for var expansion
		}
		else
		{
			buffer_size++;
			j++;
		}
	}
	
	// Add safety margin
	buffer_size = buffer_size + 1024;
	
	// Allocate space for the word
	word = (char *)malloc((buffer_size + 1) * sizeof(char));
	if (!word)
		return (1);
	
	// Now extract the word with proper quote handling and expansion
	j = 0;
	while (input[*i] && input[*i] != ' ' && input[*i] != '\t' &&
		   input[*i] != '|' && input[*i] != '<' && input[*i] != '>' &&
		   input[*i] != '(' && input[*i] != ')' &&
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
			var_name_len = 0;
			while (input[*i + var_name_len] && (ft_isalnum(input[*i + var_name_len]) || input[*i + var_name_len] == '_' || 
				   (var_name_len == 0 && input[*i + var_name_len] == '?')))
				var_name_len++;
			
			var_name = ft_substr(input, *i, var_name_len);
			if (!var_name)
			{
				free(word);
				return (1);
			}
			
			if (ft_strcmp(var_name, "?") == 0)
			{
				// Handle $? for last exit status
				char *exit_status;
				exit_status = ft_itoa(shell->exit_status);
				if (!exit_status)
				{
					free(var_name);
					free(word);
					return (1);
				}
				var_value = exit_status; // This will be freed later
			}
			else
			{
				var_value = get_env_value(shell, var_name);
				if (var_value)
					var_value = ft_strdup(var_value);
				else
					var_value = ft_strdup("");
			}
			
			if (!var_value)
			{
				free(var_name);
				free(word);
				return (1);
			}
			
						value_len = ft_strlen(var_value);
			
			// Make sure we don't exceed buffer size
			if (j + (int)value_len >= buffer_size)
			{
				free(var_name);
				free(var_value);
				free(word);
				return (1);
			}
			
			ft_strlcpy(word + j, var_value, value_len + 1);
			j += value_len;
			
			free(var_name);
			free(var_value);
			*i += var_name_len;
		}
		else
		{
			// Make sure we don't exceed buffer size
			if (j >= buffer_size)
			{
				free(word);
				return (1);
			}
			word[j++] = input[(*i)++];
		}
	}
	
	// Make sure we don't exceed buffer size
	if (j >= buffer_size)
	{
		free(word);
		return (1);
	}
	
	word[j] = '\0';
	
	// For complex words, don't split - just add as single token
	add_token(&shell->tokens, create_token(word, TOKEN_WORD));
	free(word);
	return (0);
}

bool	tokenize_input(t_shell *shell, char *input)
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
			{

				return (false);
			}
	}
	return (true);
}
