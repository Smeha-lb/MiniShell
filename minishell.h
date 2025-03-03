#include "libs/libft/libft.h"

typedef struct env_list{
    char *name;
    char *value;
    char *data;
    struct env_list *next;
}env_list;


env_list *create_env_list(char **env);


int check_list_dup(char *data, env_list *head);


void add_env_var(char *data, env_list *head);
void print_env(env_list *head);


char *ft_expand(char *name, env_list *head);

