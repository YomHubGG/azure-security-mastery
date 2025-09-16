/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_node_creation_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 08:40:06 by tritter           #+#    #+#             */
/*   Updated: 2025/07/04 19:26:21 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*create_lor_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node			*node;
	t_logical_or_token	*lor;

	lor = g_malloc(data->hot_zone, sizeof(t_logical_or_token));
	if (!lor)
		return (NULL);
	lor->left = left;
	lor->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = LOGICAL_OR;
	node->token.logical_or = lor;
	return (node);
}

t_tree_node	*create_land_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node			*node;
	t_logical_and_token	*land;

	land = g_malloc(data->hot_zone, sizeof(t_logical_and_token));
	if (!land)
		return (NULL);
	land->left = left;
	land->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = LOGICAL_AND;
	node->token.logical_and = land;
	return (node);
}

t_tree_node	*create_hdoc_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node			*node;
	t_heredoc_token		*heredoc;

	heredoc = g_malloc(data->hot_zone, sizeof(t_heredoc_token));
	if (!heredoc)
		return (NULL);
	heredoc->left = left;
	heredoc->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = HERE_DOC;
	node->token.heredoc = heredoc;
	return (node);
}

t_tree_node	*create_append_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node			*node;
	t_append_token		*append;

	if (!left || !right)
		return (NULL);
	append = g_malloc(data->hot_zone, sizeof(t_append_token));
	if (!append)
		return (NULL);
	append->left = left;
	append->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = APPEND;
	node->token.append = append;
	return (node);
}

t_tree_node	*create_error_node(t_token *leftovers,
	t_error_type error_type, t_data *data)
{
	t_tree_node			*node;
	t_error_token		*error;

	error = g_malloc(data->hot_zone, sizeof(t_error_token));
	if (!error)
		return (NULL);
	error->error = error_type;
	error->leftovers = leftovers;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = ERROR;
	node->token.error = error;
	return (node);
}
