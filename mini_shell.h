#include "42-libft/libft.h"

typedef struct env_list{
    char *name;
    char *value;
    char *data;
    struct env_list *next;
}env_list;

