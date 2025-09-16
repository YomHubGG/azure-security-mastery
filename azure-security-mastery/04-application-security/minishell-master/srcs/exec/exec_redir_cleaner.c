/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_cleaner.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 21:03:13 by tritter           #+#    #+#             */
/*   Updated: 2025/07/06 18:57:58 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_redir_list	*new_redir_store_node(t_data *data, int redir)
{
	t_redir_list	*ret;

	ret = g_malloc(data->hot_zone, sizeof(t_redir_list));
	ret->next = NULL;
	ret->redir = redir;
	return (ret);
}

t_redir_list	*last_redir_store_node(t_data *data)
{
	t_redir_list	*curr;

	if (!data->redir_list)
		return (NULL);
	curr = data->redir_list;
	while (curr->next)
		curr = curr->next;
	return (curr);
}

void	redir_store_add_back(t_data *data, t_redir_list *new)
{
	t_redir_list	*last;

	if (!data->redir_list)
	{
		data->redir_list = new;
		return ;
	}
	last = last_redir_store_node(data);
	last->next = new;
}

void	close_all_redir_fds_except(t_data *data, int keep_read, int keep_write)
{
	t_redir_list	*node;

	node = data->redir_list;
	while (node)
	{
		if (node->redir != keep_read && node->redir != keep_write)
		{
			if (node->redir != -1)
				close(node->redir);
			node->redir = -1;
		}
		node = node->next;
	}
}
