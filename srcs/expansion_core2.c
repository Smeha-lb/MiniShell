#include "../includes/minishell.h"

char	*expand_variables_core(t_shell *shell, const char *str)
{
	int						i;
	int						j;
	char					*expanded;
	t_var_expansion_data	data;

	if (!str)
		return (NULL);
	expanded = (char *)malloc((calculate_expanded_size
				(shell, str) + 1) * sizeof(char));
	if (!expanded)
		return (NULL);
	i = 0;
	j = 0;
	data.str = str;
	data.i = &i;
	data.expanded = expanded;
	data.j = &j;
	init_expansion_data(&data);
	process_expansion_loop(shell, &data);
	expanded[j] = '\0';
	return (expanded);
}

char	*expand_token(t_shell *shell, const char *token)
{
	if (!token || !*token)
		return (NULL);
	return (expand_variables_core(shell, token));
}
