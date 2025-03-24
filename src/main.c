/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 13:02:12 by moabdels          #+#    #+#             */
/*   Updated: 2025/03/24 15:03:16 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
#include "../inc/ft_memsafety.h"

int main(int argc, char **argv, char **envp) {
    // creat a function to turn env to a linked list
    (void)argc;
    (void)argv;
    (void)envp;
    ft_printf("is 16 a power of 2: %d", is_power_of_two(16));
    return 0;
}
