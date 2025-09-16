/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 16:39:59 by tritter           #+#    #+#             */
/*   Updated: 2025/07/23 16:51:30 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	insert_tree_node(t_tree_node **n, t_tree_node *new_t_node,
	t_tree_node *t_current, int i)
{
	if (i == 0)
		(*n)->token.word->word_subtype = new_t_node->token.word->word_subtype;
	else
	{
		new_t_node->token.word->next_word = t_current->token.word->next_word;
		t_current->token.word->next_word = new_t_node;
	}
}

//to comp should contain atleast one * token otherwise its undefined behavior
//New implementation that properly handles multiple wildcards
bool	wildcard_match(char *pattern, char *text)
{
	if (!pattern || !text)
		return (false);
	if (*pattern == '\0')
		return (*text == '\0');
	if (*pattern == '*')
	{
		while (*pattern == '*')
			pattern++;
		if (*pattern == '\0')
			return (true);
		while (*text != '\0')
		{
			if (wildcard_match(pattern, text))
				return (true);
			text++;
		}
		return (wildcard_match(pattern, text));
	}
	if (*text != '\0' && *pattern == *text)
		return (wildcard_match(pattern + 1, text + 1));
	return (false);
}

t_token	*token_divider(t_token *old, char *value, t_data *data)
{
	t_token	*new_tok;

	new_tok = g_malloc(data->hot_zone, sizeof(t_token));
	if (!new_tok)
		return (NULL);
	new_tok->fully_quoted = old->fully_quoted;
	new_tok->joined = old->joined;
	new_tok->next = NULL;
	new_tok->prev = NULL;
	new_tok->quote_type = old->quote_type;
	new_tok->quoted = old->quoted;
	new_tok->type = old->type;
	new_tok->value = ft_strdup(value);
	insert_malloc(data->hot_zone, new_tok->value);
	if (!new_tok->value)
		return (NULL);
	return (new_tok);
}
