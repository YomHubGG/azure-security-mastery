/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:22:59 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 15:47:45 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_token_type	pre_tokenize(char c, char next)
{
	if (c == '&' && next == '&')
		return (TOKEN_LOGICAL_AND);
	if (c == '|' && next == '|')
		return (TOKEN_LOGICAL_OR);
	if (c == '<' && next == '<')
		return (TOKEN_HEREDOC);
	if (c == '>' && next == '>')
		return (TOKEN_APPEND);
	if (c == '$' && next == '?')
		return (TOKEN_EXIT_STATUS);
	return (pre_tokenize_b(c));
}

t_token_type	pre_tokenize_b(char c)
{
	if (c == '*')
		return (TOKEN_ASTERISK);
	if (c == '?')
		return (TOKEN_INT_MARK);
	if (c == ' ' || c == '\t' || c == '\v')
		return (TOKEN_SPACE);
	if (c == '|')
		return (TOKEN_BAR);
	if (c == '<')
		return (TOKEN_LESS);
	if (c == '>')
		return (TOKEN_GREATER);
	if (c == '&')
		return (TOKEN_AMPERSAND);
	return (pre_tokenize_c(c));
}

t_token_type	pre_tokenize_c(char c)
{
	if (c == '$')
		return (TOKEN_DOLLAR);
	if (c == '~')
		return (TOKEN_TILDE);
	if (c == '(')
		return (TOKEN_L_PARENTHESIS);
	if (c == ')')
		return (TOKEN_R_PARENTHESIS);
	if (c == '\'')
		return (TOKEN_SINGLE_QUOTE);
	if (c == '\"')
		return (TOKEN_DOUBLE_QUOTE);
	return (TOKEN_CHAR);
}

t_pre_token	*create_pre_token_node(t_data *data)
{
	t_pre_token	*first_node;

	first_node = g_malloc(data->hot_zone, sizeof(t_pre_token));
	if (!first_node)
		return (NULL);
	first_node->character = '\0';
	first_node->type = TOKEN_CHAR;
	first_node->str = NULL;
	first_node->in_squote = false;
	first_node->in_dquote = false;
	first_node->next = NULL;
	first_node->prev = NULL;
	return (first_node);
}

t_pre_token	*add_pre_token_node(t_data *data, t_pre_token *prev)
{
	t_pre_token	*new;

	new = g_malloc(data->hot_zone, sizeof(t_pre_token));
	if (!new)
		return (NULL);
	new->character = '\0';
	new->type = TOKEN_CHAR;
	new->str = NULL;
	new->in_squote = false;
	new->in_dquote = false;
	new->next = NULL;
	new->prev = prev;
	prev->next = new;
	return (new);
}
