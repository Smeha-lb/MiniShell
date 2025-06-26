#include "../includes/minishell.h"

void	add_expanded_tokens(t_shell *shell, char *expanded_value)
{
	char	**split_values;
	int		i;

	if (ft_strchr(expanded_value, ' '))
	{
		split_values = ft_split(expanded_value, ' ');
		if (split_values)
		{
			i = 0;
			while (split_values[i])
			{
				if (ft_strlen(split_values[i]) > 0)
					add_to_token_list(&shell->tokens,
						create_token(split_values[i], TOKEN_WORD, 0));
				i++;
			}
			free_array(split_values);
		}
	}
	else
	{
		if (ft_strlen(expanded_value) > 0)
			add_to_token_list(&shell->tokens,
				create_token(expanded_value, TOKEN_WORD, 0));
	}
}

char	*get_variable_value(t_shell *shell, char *var_name)
{
	char	*var_value;

	if (ft_strcmp(var_name, "?") == 0)
	{
		var_value = ft_itoa(shell->exit_status);
	}
	else
	{
		var_value = get_env_value(shell, var_name);
		if (var_value)
			var_value = ft_strdup(var_value);
		else
			var_value = ft_strdup("");
	}
	return (var_value);
}

int	is_var_delimiter(char c, char next_c)
{
	if (c == '\0')
		return (1);
	return (c == ' ' || c == '\t'
		|| c == '|' || c == '<' || c == '>'
		|| c == '(' || c == ')'
		|| (c == '&' && next_c == '&'));
}

int	get_var_name_len(char *input)
{
	int	len;

	len = 0;
	while (input[len] && (ft_isalnum(input[len])
			|| input[len] == '_'
			|| (len == 0 && input[len] == '?')))
		len++;
	return (len);
}
