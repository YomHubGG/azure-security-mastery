/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redirection_handler_subwrapper.c               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 16:01:21 by tritter           #+#    #+#             */
/*   Updated: 2025/07/19 14:52:33 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*redir_append_subwrapper(t_tree_node *new_node, t_token **token,
	t_token *current, t_data *data)
{
	new_node = create_append_node(create_word_node(current->next, data),
			parse_redir(rm_redir(token, TOKEN_APPEND, current), data), data);
	if (!new_node)
		return (NULL);
	return (new_node);
}

t_tree_node	*redir_rout_subwrapper(t_tree_node *new_node, t_token **token,
	t_token *current, t_data *data)
{
	new_node = create_redirect_out_node(create_word_node(current->next, data),
			parse_redir(rm_redir(token, TOKEN_GREATER, current), data), data);
	if (!new_node)
		return (NULL);
	return (new_node);
}

t_tree_node	*redir_rin_subwrapper(t_tree_node *new_node, t_token **token,
	t_token *current, t_data *data)
{
	new_node = create_redirect_in_node(create_word_node(current->next, data),
			parse_redir(rm_redir(token, TOKEN_LESS, current), data), data);
	if (!new_node)
		return (NULL);
	return (new_node);
}

t_tree_node	*parse_heredoc_subwrapper(t_token *current, t_tree_node *new_node,
	t_token **token, t_data *data)
{
	new_node = create_hdoc_node(create_word_node(current->next, data),
			parse_redir(rm_redir(token, TOKEN_HEREDOC, current), data), data);
	if (!new_node)
		return (NULL);
	return (new_node);
}
