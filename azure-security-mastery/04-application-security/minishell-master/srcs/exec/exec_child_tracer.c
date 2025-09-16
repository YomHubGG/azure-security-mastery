/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_tracer.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 19:53:38 by tritter           #+#    #+#             */
/*   Updated: 2025/07/06 18:46:08 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_pid_list	*new_pid_node(t_data *data, pid_t new_pid)
{
	t_pid_list	*ret;

	ret = g_malloc(data->hot_zone, sizeof(t_pid_list));
	ret->next = NULL;
	ret->pid = new_pid;
	return (ret);
}

t_pid_list	*last_pid_node(t_data *data)
{
	t_pid_list	*curr;

	if (!data->pid_list)
		return (NULL);
	curr = data->pid_list;
	while (curr->next)
		curr = curr->next;
	return (curr);
}

void	pid_add_back(t_data *data, t_pid_list *new)
{
	t_pid_list	*last;

	if (!data->pid_list)
	{
		data->pid_list = new;
		return ;
	}
	last = last_pid_node(data);
	last->next = new;
}

void	add_trace(t_data *data, pid_t new_pid)
{
	t_pid_list	*new;

	new = new_pid_node(data, new_pid);
	pid_add_back(data, new);
}
