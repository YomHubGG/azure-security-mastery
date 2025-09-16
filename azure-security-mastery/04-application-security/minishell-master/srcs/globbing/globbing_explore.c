/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing_explore.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 12:25:23 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 20:27:04 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	globbing_explore_tree(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (!node)
		return (0);
	if (node->type == WORD)
	{
		if (node->token.word->word_subtype->type == TOKEN_WILDCARD)
			r_code = (init_globbing(&node, data));
		if (r_code == GLOBBING_MALLOC_ERR)
			return (r_code);
		else
			return (globbing_explore_tree(node->token.word->next_word, data));
	}
	if (node->type == HERE_DOC)
		return (globbing_explore_tree(node->token.heredoc->right, data));
	if (node->type == PARENTHESIS_P)
		return (globbing_explore_tree(node->token.parenthesis->subtree, data));
	return (globbing_explore_tree_2(node, data));
}

int	globbing_explore_tree_2(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == ERROR)
		return (GLOBBING_UNVALID_AST);
	if (node->type == PIPE)
	{
		r_code = globbing_explore_tree(node->token.pipe->word1, data);
		if (r_code == GLOBBING_MALLOC_ERR)
			return (r_code);
		else
			return (globbing_explore_tree(node->token.pipe->word2, data));
	}
	if (node->type == REDIRECT_IN)
		return (globbing_explore_tree(node->token.redirect_in->right, data));
	return (globbing_explore_tree_3(node, data));
}

int	globbing_explore_tree_3(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == REDIRECT_OUT)
		return (globbing_explore_tree(node->token.redirect_out->right, data));
	if (node->type == LOGICAL_AND)
	{
		r_code = globbing_explore_tree(node->token.logical_and->left, data);
		if (r_code == GLOBBING_MALLOC_ERR)
			return (r_code);
		else
			return (globbing_explore_tree
				(node->token.logical_and->right, data));
	}
	return (globbing_explore_tree_4(node, data));
}

int	globbing_explore_tree_4(t_tree_node *node, t_data *data)
{
	int	r_code;

	r_code = 0;
	if (node->type == LOGICAL_OR)
	{
		r_code = globbing_explore_tree(node->token.logical_or->left, data);
		if (r_code == GLOBBING_MALLOC_ERR)
			return (r_code);
		r_code = globbing_explore_tree(node->token.logical_or->right, data);
		if (r_code == GLOBBING_MALLOC_ERR)
			return (r_code);
		return (0);
	}
	if (node->type == APPEND)
		return (globbing_explore_tree(node->token.append->right, data));
	return (GLOBBING_UNKNOWN_AST_NODE_TYPE);
}
