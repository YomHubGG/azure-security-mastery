/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_explore.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 09:37:14 by tritter           #+#    #+#             */
/*   Updated: 2025/08/05 16:34:27 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_explore(t_data *data, t_tree_node *ast, int read, int write)
{
	if (!ast || read <= -1 || write <= -1)
	{
		if (read == -2 || write == -2)
			return (set_last_exit_status(data, 130));
		else
			return (set_last_exit_status(data, 0));
	}
	else if (ast->type == REDIRECT_OUT)
		exec_redir_out_subwraper(data, ast, read, write);
	else if (ast->type == REDIRECT_IN)
		exec_redir_in_subwraper(data, ast, read, write);
	else
		exec_explore2(data, ast, read, write);
}

void	exec_explore2(t_data *data, t_tree_node *ast, int read, int write)
{
	t_mini_fd	fdx;

	fdx.read = read;
	fdx.write = write;
	if (ast->type == WORD)
	{
		exec_clear_quote(data, ast);
		if (!ast->token.word->word_subtype->value
			|| ast->token.word->word_subtype->value[0] == '\0')
			return (set_last_exit_status(data, 0));
		if (data->can_exec == true)
		{
			if (is_builtin(ast->token.word->word_subtype->value))
			{
				data->last_exit_status = execute_builtin
					(ast->token.word->word_subtype->value,
						create_args(data, ast), data, write);
				return (safe_close(write, read));
			}
			else
				init_exec(data, ast, fdx);
		}
	}
	else
		exec_explore3(data, ast, read, write);
}

void	exec_explore3(t_data *data, t_tree_node *ast, int read, int write)
{
	if (ast->type == LOGICAL_AND)
	{
		exec_explore(data, ast->token.logical_and->left, read, write);
		wait_for_all_children(data);
		if (data->last_exit_status != 0)
			data->can_exec = false;
		else
			data->can_exec = true;
		exec_explore(data, ast->token.logical_and->right, read, write);
	}
	else if (ast->type == LOGICAL_OR)
	{
		exec_explore(data, ast->token.logical_or->left, read, write);
		wait_for_all_children(data);
		if (data->last_exit_status == 0)
			data->can_exec = !data->can_exec;
		else
			data->can_exec = true;
		exec_explore(data, ast->token.logical_or->right, read, write);
	}
	else
		exec_explore4(data, ast, read, write);
}

void	exec_explore4(t_data *data, t_tree_node *ast, int read, int write)
{
	if (ast->type == HERE_DOC)
	{
		if (ast->token.heredoc->premade_fd != -1)
			read = ast->token.heredoc->premade_fd;
		else
			return ;
		exec_explore(data, ast->token.heredoc->right, read, write);
		if (read != STDOUT_FILENO && read != STDIN_FILENO)
			close(read);
	}
	else if (ast->type == APPEND)
	{
		exec_clear_quote(data, ast->token.append->left);
		write = open_file(ast->token.append->left, O_APPEND | O_WRONLY);
		if (write == -1)
			return (set_last_exit_status(data, 1));
		redir_store_add_back(data, new_redir_store_node(data, write));
		exec_explore(data, ast->token.append->right, read, write);
		if (write != STDOUT_FILENO)
			close(write);
	}
	else
		exec_explore5(data, ast, read, write);
}

void	exec_explore5(t_data *data, t_tree_node *ast, int read, int write)
{
	int			p[2];
	t_pipe_list	*p_store;

	if (ast->type == PIPE)
	{
		if (pipe(p) == -1)
			return (perror("pipe failure"));
		p_store = new_pipe_store_node(data, p[0], p[1]);
		pipe_store_add_back(data, p_store);
		data->is_piped = true;
		exec_explore(data, ast->token.pipe->word1, read, p[1]);
		close(p[1]);
		exec_explore(data, ast->token.pipe->word2, p[0], write);
		close(p[0]);
	}
	else if (ast->type == PARENTHESIS_P)
	{
		global_child_fd_cleanup(data, read, write);
		subshell_handler_init(ast, data, read, write);
	}
	else if (ast->type == ERROR)
		return (set_last_exit_status(data, 2));
	else
		printf(RED"UNKNOWN NODE TYPE\n");
}
