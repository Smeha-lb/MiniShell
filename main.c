#include "mini_shell.h"

int main(int argc, char **argv, char **envp){
    (void) argv;
    (void) argc;
    env_list *head = make_linked_list(envp);
	add_env_var("TEST=123", head);
	print_env(head);
    return(0);
}