/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_word_handler.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 08:45:41 by tritter           #+#    #+#             */
/*   Updated: 2025/08/05 13:28:30 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_tree_node	*parse_word(t_token **token, t_data *data)
{
	t_pwa	acc;

	if (!token)
		return (NULL);
	acc.word_h = g_malloc(data->hot_zone, sizeof(t_tree_node *));
	if (!acc.word_h)
		return (NULL);
	(*acc.word_h) = NULL;
	acc.current = *token;
	while (acc.current && is_word_subtype(acc.current->type))
	{
		if (acc.current->next && !is_word_subtype(acc.current->next->type))
			return (create_error_node(*token, INVALID_WORD, data));
		acc.new_word = create_word_node(acc.current, data);
		if (!acc.new_word)
			return (NULL);
		if (!(*acc.word_h))
			(*acc.word_h) = acc.new_word;
		else
			word_add_back(acc.word_h, acc.new_word);
		acc.current = acc.current->next;
	}
	return ((*acc.word_h));
}

bool	is_word_subtype(t_token_type type)
{
	return (type == TOKEN_EXIT_STATUS || type == TOKEN_EXPAND_VARIABLE
		|| type == TOKEN_TILDE_EXPANSION || type == TOKEN_WILDCARD
		|| type == TOKEN_WORD || type == TOKEN_HEREDOC_DELIM
		|| type == TOKEN_HEREDOC_DELIM_QUOTED);
}

void	word_add_back(t_tree_node **words, t_tree_node *new_word)
{
	t_tree_node	*last;

	if (words)
	{
		if (*words)
		{
			last = ft_last_word(*words);
			last->token.word->next_word = new_word;
		}
		else
			*words = new_word;
	}
}

t_tree_node	*ft_last_word(t_tree_node *lst)
{
	if (!lst)
		return (NULL);
	while (lst->token.word->next_word)
		lst = lst->token.word->next_word;
	return (lst);
}
