/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_dispatcher.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:50:20 by ggrisole          #+#    #+#             */
/*   Updated: 2025/08/06 13:32:03 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_builtin_type	get_builtin_type(const char *cmd)
{
	if (ft_strcmp(cmd, "echo") == 0)
		return (BUILTIN_ECHO);
	else if (ft_strcmp(cmd, "cd") == 0)
		return (BUILTIN_CD);
	else if (ft_strcmp(cmd, "pwd") == 0)
		return (BUILTIN_PWD);
	else if (ft_strcmp(cmd, "env") == 0)
		return (BUILTIN_ENV);
	else if (ft_strcmp(cmd, "exit") == 0)
		return (BUILTIN_EXIT);
	else if (ft_strcmp(cmd, "export") == 0)
		return (BUILTIN_EXPORT);
	else if (ft_strcmp(cmd, "unset") == 0)
		return (BUILTIN_UNSET);
	return (BUILTIN_NONE);
}

int	is_builtin(const char *cmd)
{
	return (get_builtin_type(cmd) != BUILTIN_NONE);
}

static int	execute_builtin_wrapper(const char *cmd, char **args,
	t_data *data, int write_to)
{
	t_builtin_type	type;
	int				ret;

	ret = 1;
	if (data->is_piped)
		global_child_fd_cleanup(data, -1, write_to);
	type = get_builtin_type(cmd);
	if (type == BUILTIN_ECHO)
		ret = builtin_echo(args, write_to);
	else if (type == BUILTIN_CD)
		ret = builtin_cd(args, data);
	else if (type == BUILTIN_PWD)
		ret = builtin_pwd(data, write_to);
	else if (type == BUILTIN_ENV)
		ret = builtin_env(data, args, write_to);
	else if (type == BUILTIN_EXIT)
		ret = builtin_exit(args, data);
	else if (type == BUILTIN_EXPORT)
		ret = builtin_export(args, data, write_to);
	else if (type == BUILTIN_UNSET)
		ret = builtin_unset(args, data);
	if (data->is_piped)
		return (g_arena_clean(data->g_arena), exit(ret), 0);
	return (ret);
}

int	execute_builtin(const char *cmd, char **args, t_data *data, int write_to)
{
	pid_t			pid;

	pid = 0;
	if (data->is_piped)
		pid = fork();
	if (pid == -1)
	{
		perror("fork failed");
		data->last_exit_status = 1003;
		return (1003);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		return (execute_builtin_wrapper(cmd, args, data, write_to));
	}
	else
	{
		add_trace(data, pid);
		if (write_to != STDOUT_FILENO)
			close(write_to);
		return (0);
	}
}
