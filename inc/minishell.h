/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:04 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/19 15:17:01 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lib/libft/libft.h"

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

