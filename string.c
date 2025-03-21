#include "mini_shell.h"

string *create_string(char *s)
{
    string *string = (struct string *)malloc(sizeof(struct string));
    string->value = s;
    string->length = ft_strlen(s);
    return string;
}

void append_string_and_char_pointer(string *s1, char *s2)
{
    char *temp;
    temp = ft_strjoin(s1->value, s2);
    free(s1->value);
    s1->value = temp;
    s1->length = ft_strlen(s1->value);
}

void append_strings(string *s1, string *s2)
{
    char *temp;
    temp = ft_strjoin(s1->value, s2->value);
    free(s1->value);
    free(s2->value);
    free(s2);
    s1->value = temp;
    s1->length = ft_strlen(s1->value);
}
