/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_node_creation_1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 08:35:08 by tritter           #+#    #+#             */
/*   Updated: 2025/08/05 17:17:09 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*create_word_node(t_token *token, t_data *data)
{
	t_word_token	*t_word;
	t_tree_node		*node;

	t_word = g_malloc(data->hot_zone, sizeof(t_word_token));
	if (!t_word)
		return (NULL);
	t_word->next_word = NULL;
	t_word->word_subtype = token;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = WORD;
	node->token.word = t_word;
	return (node);
}

t_tree_node	*create_pipe_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node		*node;
	t_pipe_token	*pipe_tok;

	pipe_tok = g_malloc(data->hot_zone, sizeof(t_pipe_token));
	if (!pipe_tok)
		return (NULL);
	pipe_tok->word1 = left;
	pipe_tok->word2 = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = PIPE;
	node->token.pipe = pipe_tok;
	return (node);
}

t_tree_node	*create_redirect_in_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node		*node;
	t_redirect_in	*r_in;

	r_in = g_malloc(data->hot_zone, sizeof(t_redirect_in));
	if (!r_in)
		return (NULL);
	r_in->left = left;
	r_in->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = REDIRECT_IN;
	node->token.redirect_in = r_in;
	return (node);
}

t_tree_node	*create_redirect_out_node(t_tree_node *left,
	t_tree_node *right, t_data *data)
{
	t_tree_node		*node;
	t_redirect_out	*r_out;

	r_out = g_malloc(data->hot_zone, sizeof(t_redirect_out));
	if (!r_out)
		return (NULL);
	r_out->left = left;
	r_out->right = right;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = REDIRECT_OUT;
	node->token.redirect_out = r_out;
	return (node);
}

t_tree_node	*create_pareng_node(t_tree_node *subtree, t_data *data)
{
	t_tree_node					*node;
	t_parenthesis_placeholder	*p_holder;

	p_holder = g_malloc(data->hot_zone, sizeof(t_parenthesis_placeholder));
	if (!p_holder)
		return (NULL);
	p_holder->subtree = subtree;
	node = g_malloc(data->hot_zone, sizeof(t_tree_node));
	if (!node)
		return (NULL);
	node->type = PARENTHESIS_P;
	node->token.parenthesis = p_holder;
	return (node);
}
