/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: moabdels <moabdels@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 16:28:24 by moabdels          #+#    #+#             */
/*   Updated: 2025/04/13 15:12:07 by moabdels         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

void	reset_list(t_gnl_list **list, t_gnl_list *clean_node, char *str, int fd)
{
	t_gnl_list	*temp_node;

	clean_node->str_buf = str;
	clean_node->node_fd = fd;
	if (list == NULL || *list == NULL)
	{
		clean_node->next = NULL;
		return ;
	}
	temp_node = *list;
	while (*list && (*list)->node_fd == fd)
	{
		temp_node = (*list)->next;
		free((*list)->str_buf);
		free((*list));
		*list = temp_node;
	}
	clean_node->next = temp_node;
	if (clean_node->str_buf[0])
		*list = clean_node;
	else
	{
		free(str);
		free(clean_node);
	}
}

int	found_newline(t_gnl_list *list_node)
{
	int	i_index;

	if (list_node == NULL)
		return (0);
	while (list_node)
	{
		i_index = 0;
		while (list_node->str_buf[i_index] && i_index < BUFFER_SIZE)
		{
			if (list_node->str_buf[i_index] == '\n')
				return (1);
			i_index++;
		}
		list_node = list_node->next;
	}
	return (0);
}

t_gnl_list	*get_last_node(t_gnl_list *list_node)
{
	if (list_node == NULL)
		return (NULL);
	while (list_node->next)
		list_node = list_node->next;
	return (list_node);
}

void	copy_str_to_node(t_gnl_list *list_node, char *str, int fd)
{
	int	i_node_str_index;
	int	j_input_str_index;

	if (list_node == NULL)
		return ;
	j_input_str_index = 0;
	while (list_node && list_node->node_fd == fd)
	{
		i_node_str_index = 0;
		while (list_node->str_buf[i_node_str_index])
		{
			if (list_node->str_buf[i_node_str_index] == '\n')
			{
				str[j_input_str_index++] = '\n';
				str[j_input_str_index] = '\0';
				return ;
			}
			str[j_input_str_index++] = list_node->str_buf[i_node_str_index++];
		}
		list_node = list_node->next;
	}
	str[j_input_str_index] = '\0';
}

int	len_to_next_line(t_gnl_list *list_node, int fd)
{
	int	index;
	int	len;

	if (list_node == NULL)
		return (0);
	len = 0;
	while (list_node && list_node->node_fd == fd)
	{
		index = 0;
		while (list_node->str_buf[index])
		{
			if (list_node->str_buf[index] == '\n')
				return (++len);
			index++;
			len++;
		}
		list_node = list_node->next;
	}
	return (len);
}

// ! DELETE WHEN DONE

// void	print_list(t_list **list)
// {
// 	t_list	*temp;

// 	temp = *list;
// 	printf("\033[93mCURRENT LIST : \n\033[0m *\n");
// 	while (temp)
// 	{
// 		printf(" ⮡ [%s] <%i>\n", temp->str_buf, temp->node_fd);
// 		temp = temp->next;
// 	}
// }
