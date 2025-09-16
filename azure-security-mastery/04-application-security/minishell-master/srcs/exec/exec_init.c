/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:27:00 by tritter           #+#    #+#             */
/*   Updated: 2025/07/23 16:16:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	init_exec(t_data *data, t_tree_node *current, t_mini_fd fdx)
{
	char	**paths;
	char	*exec_path;

	if (current->token.word->word_subtype->value[0] == '/'
		|| (current->token.word->word_subtype->value[0] == '.'
			&&current->token.word->word_subtype->value[1] == '/'))
	{
		exec_path = current->token.word->word_subtype->value;
		return (init_helper(data, current, fdx, exec_path));
	}
	paths = get_paths(data);
	if (!paths)
		return (clean_exec_error_exit(fdx, 127, data,
				current->token.word->word_subtype->value));
	exec_path = discover_exec_path(data,
			current->token.word->word_subtype->value, paths);
	if (!exec_path)
		return (clean_exec_error_exit(fdx, 126, data,
				current->token.word->word_subtype->value));
	create_child(data, exec_path, create_args(data, current), fdx);
}

void	clean_exec_error_exit(t_mini_fd fdx, int code, t_data *data,
								char *errinfo)
{
	if (fdx.read != STDIN_FILENO)
		close(fdx.read);
	if (fdx.write != STDOUT_FILENO)
		close(fdx.write);
	return (exec_error(
			data, code, errinfo));
}

void	init_helper(t_data *data, t_tree_node *current, t_mini_fd fdx,
					char	*exec_path)
{
	if (access(exec_path, X_OK) == 0)
	{
		create_child(data, exec_path, create_args(data, current), fdx);
		return ;
	}
	else
	{
		if (access(exec_path, F_OK) != 0)
			ft_dprintf(2, "%s: No such file or directory\n", exec_path);
		else
		{
			ft_dprintf(2, "%s: Permission denied\n", exec_path);
			set_last_exit_status(data, 126);
		}
		if (fdx.read != STDIN_FILENO)
			close(fdx.read);
		if (fdx.write != STDOUT_FILENO)
			close(fdx.write);
		return (exec_error(data, 127,
				current->token.word->word_subtype->value));
	}
}

void	exec_error(t_data *data, int error_code, char *str)
{
	data->last_exit_status = error_code;
	if (error_code == 126)
	{
		data->last_exit_status = 127;
		ft_dprintf(2, "%s: No such file or directory\n", str);
		return ;
	}
	if (error_code == 127)
	{
		ft_dprintf(2, "%s: command not found\n", str);
		return ;
	}
}
