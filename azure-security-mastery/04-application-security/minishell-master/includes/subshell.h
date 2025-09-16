/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subshell.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 12:57:28 by tritter           #+#    #+#             */
/*   Updated: 2025/07/28 17:24:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//subshell_handler_init.c
void	subshell_handler_init(t_tree_node *node, t_data *data,
			int read, int write);
void	subshell_handler_init_helper(t_tree_node *node, t_data *data,
			int read, int write);
void	pipe_handler_init(t_tree_node *node, t_data *data, int read, int write);
void	pipe_handler_init_helper(t_tree_node *node, t_data *data,
			int read, int write);
