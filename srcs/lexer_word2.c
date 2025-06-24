#include "../includes/minishell.h"

int	is_word_delimiter(char c, char next_c)
{
	return (c == ' ' || c == '\t' || c == '|' || c == '<' || c == '>'
		|| c == '(' || c == ')' || (c == '&' && next_c == '&'));
}
