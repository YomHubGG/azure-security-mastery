/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subshell_handler_init.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 12:57:16 by tritter           #+#    #+#             */
/*   Updated: 2025/07/29 15:36:43 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "subshell.h"

void	subshell_handler_init(t_tree_node *node, t_data *data,
	int read, int write)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		data->last_exit_status = 1003;
		return ;
	}
	if (pid == 0)
		subshell_handler_init_helper(node, data, read, write);
	else
	{
		add_trace(data, pid);
		if (read != STDIN_FILENO)
			close(read);
		if (write != STDOUT_FILENO)
			close(write);
	}
}

void	subshell_handler_init_helper(t_tree_node *node, t_data *data,
	int read, int write)
{
	int	tempex;

	if (read != STDIN_FILENO)
	{
		dup2(read, STDIN_FILENO);
		close(read);
	}
	if (write != STDOUT_FILENO)
	{
		dup2(write, STDOUT_FILENO);
		close(write);
	}
	exec_explore(data, node->token.parenthesis->subtree,
		STDIN_FILENO, STDOUT_FILENO);
	wait_for_all_children(data);
	fd_nuke(data);
	tempex = data->last_exit_status;
	g_arena_clean(data->g_arena);
	exit(tempex);
}
