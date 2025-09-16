/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_redirection_handler.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 08:41:43 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 11:54:31 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static inline bool	is_redir(t_token_type type)
{
	return (type == TOKEN_APPEND || type == TOKEN_LESS
		|| type == TOKEN_GREATER);
}

t_tree_node	*parse_redir(t_token **token, t_data *data)
{
	t_token		*cur;
	t_tree_node	*new_node;

	new_node = NULL;
	if (!token)
		return (NULL);
	cur = *token;
	if (is_parentheses_surounding(cur))
		return (parse_group(token, data));
	while (cur)
	{
		if ((is_redir(cur->type) && (!cur->next
					|| !is_word_subtype(cur->next->type))))
			return (create_error_node(*token, INVALID_REDIRECTION, data));
		if (cur->type == TOKEN_APPEND)
			return (redir_append_subwrapper(new_node, token, cur, data));
		if (cur->type == TOKEN_GREATER)
			return (redir_rout_subwrapper(new_node, token, cur, data));
		if (cur->type == TOKEN_LESS)
			return (redir_rin_subwrapper(new_node, token, cur, data));
		if (cur->type == TOKEN_HEREDOC)
			return (parse_heredoc_subwrapper(cur, new_node, token, data));
		cur = cur->next;
	}
	return (parse_word(token, data));
}

t_token	**rm_redir(t_token **token, t_token_type type, t_token *curr)
{
	t_token	*target;
	t_token	*next;

	if (token_counter(token) == 2)
		return (NULL);
	next = NULL;
	target = curr->next;
	curr = *token;
	while (curr)
	{
		if (curr->type == type)
		{
			return (rm_redir_helper(curr, target, next, token));
		}
		curr = curr->next;
	}
	return (token);
}

t_token	**rm_redir_helper(t_token *curr, t_token *target, t_token *next,
	t_token **token)
{
	if (target)
		next = target->next;
	if (curr == *token)
	{
		*token = next;
		if (next)
			next->prev = NULL;
	}
	else
	{
		if (curr->prev)
			curr->prev->next = next;
		if (next)
			next->prev = curr->prev;
	}
	return (token);
}

int	token_counter(t_token **token)
{
	int		ret;
	t_token	*current;

	current = *token;
	ret = 0;
	while (current)
	{
		current = current->next;
		ret++;
	}
	return (ret);
}
