/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 14:04:29 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 12:48:27 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//Explore the tree recursively until it encounters a word to expand
int	expand_explore_tree(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (!node)
		return (0);
	if (node->type == HERE_DOC)
	{
		node->token.heredoc->premade_fd = handle_heredoc
			(node->token.heredoc->left->token.word->word_subtype->value, data,
				!node->token.heredoc->left->token.word->word_subtype->quoted);
		if (node->token.heredoc->premade_fd == -1)
		{
			data->last_exit_status = 1;
			return (1);
		}
		redir_store_add_back(data,
			new_redir_store_node(data, node->token.heredoc->premade_fd));
		r_code = expand_explore_tree(node->token.heredoc->right, data);
		return (r_code);
	}
	if (node->type == PARENTHESIS_P)
		return (expand_explore_tree(node->token.parenthesis->subtree, data));
	return (expand_explore_tree_2(node, data));
}

int	expand_explore_tree_2(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == ERROR)
		return (EXPAND_UNVALID_AST);
	if (node->type == PIPE)
	{
		r_code = expand_explore_tree(node->token.pipe->word1, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.pipe->word2, data));
	}
	if (node->type == REDIRECT_IN)
	{
		r_code = init_expand_word(node->token.redirect_in->left, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.redirect_in->right, data));
	}
	return (expand_explore_tree_3(node, data));
}

int	expand_explore_tree_3(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == REDIRECT_OUT)
	{
		r_code = init_expand_word(node->token.redirect_out->left, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.redirect_out->right, data));
	}
	if (node->type == LOGICAL_AND)
	{
		r_code = expand_explore_tree(node->token.logical_and->left, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.logical_and->right, data));
	}
	return (expand_explore_tree_4(node, data));
}

int	expand_explore_tree_4(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == LOGICAL_OR)
	{
		r_code = expand_explore_tree(node->token.logical_or->left, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		r_code = expand_explore_tree(node->token.logical_or->right, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		return (0);
	}
	if (node->type == APPEND)
	{
		r_code = init_expand_word(node->token.append->left, data);
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.append->right, data));
	}
	else
		return (expand_explore_tree_5(node, data));
}

int	expand_explore_tree_5(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == WORD)
	{
		r_code = (init_expand_word(node, data));
		if (r_code == EXPAND_MALLOC_ERR)
			return (r_code);
		else
			return (expand_explore_tree(node->token.word->next_word, data));
	}
	return (EXPAND_UNKNOWN_AST_NODE_TYPE);
}
