#ifndef LEXING_H
# define LEXING_H

# include "minishell.h"


t_token	parse_token(char chr, char next);
int 	get_token_precedence(t_token token);

#endif