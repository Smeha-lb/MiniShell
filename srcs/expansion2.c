#include "../includes/minishell.h"

static int	is_variable(char c1, char c2)
{
	return (c1 == '$' && c2 && (ft_isalnum(c2) || c2 == '_'
			|| c2 == '?') && c2 != '\"');
}

char	*expand_variables(t_shell *shell, char *str, int in_quotes)
{
	int		i;
	int		j;
	char	*expanded;

	(void)in_quotes;
	if (!str)
		return (NULL);
	expanded = allocate_expanded_string(shell, str);
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (is_variable(str[i], str[i + 1]))
		{
			copy_variable(shell, str, &i, expanded + j);
			j += ft_strlen(expanded + j);
		}
		else
			process_char(str, &i, expanded, &j);
	}
	expanded[j] = '\0';
	return (expanded);
}

char	*allocate_expanded_string(t_shell *shell, char *str)
{
	char	*expanded;

	expanded = (char *)malloc((calculate_expanded_length(shell, str) + 1)
			*sizeof(char));
	return (expanded);
}

void	process_char(char *str, int *i, char *expanded, int *j)
{
	expanded[*j] = str[*i];
	(*j)++;
	(*i)++;
}
