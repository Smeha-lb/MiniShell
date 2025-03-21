#include "libs/libft/libft.h"

typedef struct env_list{
    char *name;
    char *value;
    char *data;
    struct env_list *next;
}env_list;

typedef struct string{;
    char *value;
    int length;
}string;

env_list *make_linked_list(char **env);


int check_list_dup(char *data, env_list *head);


void export(env_list *head);
void add_env_var(char *data, env_list *head);
void print_env(env_list *head);


char *expand(char *name, env_list *head);

