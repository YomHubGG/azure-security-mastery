/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_process.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:28:44 by tritter           #+#    #+#             */
/*   Updated: 2025/07/31 18:15:22 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	create_child(t_data *data,
	char *exec_path, char **args, t_mini_fd fdx)
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
	{
		signal(SIGQUIT, SIG_DFL);
		child_process(data, args, exec_path, fdx);
	}
	else
	{
		add_trace(data, pid);
		if (fdx.read != STDIN_FILENO)
			close(fdx.read);
		if (fdx.write != STDOUT_FILENO)
			close(fdx.write);
	}
}

void	ft_hat(int signum)
{
	(void)signum;
	exit(127);
}

void	child_process(t_data *data, char **args, char *exec_path, t_mini_fd fdx)
{
	char	**env_var;

	env_var = envp_list_to_array(data);
	if (!env_var)
	{
		perror("envp malloc fail");
		exit(1);
	}
	signal(SIGINT, SIG_DFL);
	global_child_fd_cleanup(data, fdx.read, fdx.write);
	if (fdx.read != STDIN_FILENO)
	{
		dup2(fdx.read, STDIN_FILENO);
		close(fdx.read);
	}
	if (fdx.write != STDOUT_FILENO)
	{
		dup2(fdx.write, STDOUT_FILENO);
		close(fdx.write);
	}
	execve(exec_path, args, env_var);
	fd_nuke(data);
	perror("command failed");
	g_arena_clean(data->g_arena);
	exit(127);
}
