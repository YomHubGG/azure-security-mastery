/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing_node.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 14:39:45 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 12:49:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	create_add_back_glob_node(char *d_name, t_data *data,
		t_glob_node **g_head)
{
	t_glob_node	*new_g_node;

	new_g_node = new_glob_node(d_name, data);
	if (!new_g_node)
		return ;
	if (!(*g_head))
	{
		(*g_head) = new_g_node;
		return ;
	}
	else
		node_glob_add_back(new_g_node, g_head);
}

t_glob_node	*new_glob_node(char *d_name, t_data *data)
{
	t_glob_node	*new_node;

	new_node = g_malloc(data->hot_zone, sizeof(t_glob_node));
	if (!new_node)
		return (NULL);
	new_node->d_name = ft_strdup(d_name);
	if (!new_node->d_name)
		return (NULL);
	insert_malloc(data->hot_zone, new_node->d_name);
	new_node->next = NULL;
	new_node->prev = NULL;
	return (new_node);
}

void	node_glob_add_back(t_glob_node *new_glob_node, t_glob_node **g_head)
{
	t_glob_node	*last;

	last = NULL;
	if (g_head)
	{
		if (!*g_head)
			*g_head = new_glob_node;
		else
		{
			last = ft_last_gn(*g_head);
			last->next = new_glob_node;
			new_glob_node->prev = last;
		}
	}
}

t_glob_node	*ft_last_gn(t_glob_node *g_head)
{
	if (!g_head)
		return (NULL);
	while (g_head->next)
		g_head = g_head->next;
	return (g_head);
}
