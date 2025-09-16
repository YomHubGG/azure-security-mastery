/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_wait_child.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 20:22:04 by tritter           #+#    #+#             */
/*   Updated: 2025/08/04 09:38:24 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	process_count(t_data *data);

void	wait_for_all_children(t_data *data)
{
	int			status;
	pid_t		waited_pid;
	int			sig;
	int			processes_left;

	if (!data->pid_list)
		return ;
	processes_left = process_count(data);
	while (processes_left > 0)
	{
		waited_pid = waitpid(-1, &status, 0);
		if (waited_pid == -1)
			break ;
		if (WIFEXITED(status))
			data->last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			sig = WTERMSIG(status);
			if (sig != SIGPIPE)
				data->last_exit_status = 128 + sig;
		}
		processes_left--;
	}
}

int	process_count(t_data *data)
{
	int			processes_left;
	t_pid_list	*curr;

	processes_left = 0;
	curr = data->pid_list;
	while (curr)
	{
		processes_left++;
		curr = curr->next;
	}
	return (processes_left);
}

void	global_child_fd_cleanup(t_data *data, int read, int write)
{
	close_all_pipe_fds_except(data, read, write);
	close_all_redir_fds_except(data, read, write);
}

void	exec_entry(t_data *data, t_tree_node *ast)
{
	signal(SIGINT, sigint_handler_2);
	exec_explore(data, ast, STDIN_FILENO, STDOUT_FILENO);
	wait_for_all_children(data);
	fd_nuke(data);
	data->pid_list = NULL;
	signal(SIGINT, sigint_handler);
}
