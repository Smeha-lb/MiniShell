#include "../includes/minishell.h"

// Store the original argument without expansion
void	add_arg(t_command *cmd, char *arg)
{
	add_arg_with_quoted(cmd, arg, 0);
}

void	add_arg_with_quoted(t_command *cmd, char *arg, int quoted)
{
	char	*arg_copy;

	arg_copy = ft_strdup(arg);
	if (!arg_copy)
		return ;
	if (!cmd->args)
		init_cmd_args(cmd, arg_copy, quoted);
	else
		append_cmd_arg(cmd, arg_copy, quoted);
}

// If wildcard expansion fails, use the original token value
// otherwise just add the argument as is
void	parse_tokens_word(char *value, t_command *cmd, int quoted)
{
	char	**matches;
	int		i;

	if (!has_wildcards(value))
		return (add_arg_with_quoted(cmd, value, quoted));
	matches = expand_wildcards(value);
	if (!matches)
		return (add_arg_with_quoted(cmd, value, quoted));
	i = 0;
	while (matches[i])
	{
		add_arg(cmd, matches[i]);
		i++;
	}
	free_array(matches);
}
