/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_redir_subwraper.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 09:48:05 by tritter           #+#    #+#             */
/*   Updated: 2025/07/29 18:01:50 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_redir_out_subwraper(t_data *data, t_tree_node *ast,
	int read, int write)
{
	exec_clear_quote(data, ast->token.redirect_out->left);
	write = open_file(ast->token.redirect_out->left, O_WRONLY);
	if (write == -1)
		return (set_last_exit_status(data, 1));
	redir_store_add_back(data, new_redir_store_node(data, write));
	exec_explore(data, ast->token.redirect_out->right, read, write);
	if (write != STDOUT_FILENO)
		close(write);
}

void	exec_redir_in_subwraper(t_data *data, t_tree_node *ast,
	int read, int write)
{
	exec_clear_quote(data, ast->token.redirect_in->left);
	read = open_file(ast->token.redirect_in->left, O_RDONLY);
	if (read == -1)
		return (set_last_exit_status(data, 1));
	redir_store_add_back(data, new_redir_store_node(data, read));
	exec_explore(data, ast->token.redirect_in->right, read, write);
	if (read != STDIN_FILENO)
		close(read);
}
