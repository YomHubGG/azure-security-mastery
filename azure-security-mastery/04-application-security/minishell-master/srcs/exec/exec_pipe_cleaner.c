/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pipe_cleaner.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 20:44:59 by tritter           #+#    #+#             */
/*   Updated: 2025/08/06 13:34:40 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_pipe_list	*new_pipe_store_node(t_data *data, int fd0, int fd1)
{
	t_pipe_list	*ret;

	ret = g_malloc(data->hot_zone, sizeof(t_pipe_list));
	if (!ret)
		return (g_arena_clean(data->g_arena), NULL);
	ret->next = NULL;
	ret->fd0 = fd0;
	ret->fd1 = fd1;
	return (ret);
}

t_pipe_list	*last_pipe_store_node(t_data *data)
{
	t_pipe_list	*curr;

	if (!data->pipe_list)
		return (NULL);
	curr = data->pipe_list;
	while (curr->next)
		curr = curr->next;
	return (curr);
}

void	pipe_store_add_back(t_data *data, t_pipe_list *new)
{
	t_pipe_list	*last;

	if (!data->pipe_list)
	{
		data->pipe_list = new;
		return ;
	}
	last = last_pipe_store_node(data);
	last->next = new;
}

void	close_all_pipe_fds_except(t_data *data, int keep_read, int keep_write)
{
	t_pipe_list	*node;

	node = data->pipe_list;
	while (node)
	{
		if (node->fd0 != keep_read && node->fd0 != keep_write)
		{
			if (node->fd0 != -1)
				close(node->fd0);
			node->fd0 = -1;
		}
		if (node->fd1 != keep_read && node->fd1 != keep_write)
		{
			if (node->fd1 != -1)
				close(node->fd1);
			node->fd1 = -1;
		}
		node = node->next;
	}
}

void	fd_nuke(t_data *data)
{
	close_all_pipe_fds_except(data, -1, -1);
	close_all_redir_fds_except(data, -1, -1);
}
