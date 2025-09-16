/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 14:27:51 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/17 15:12:34 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// join double char tokens AND trim extra whitespaces before and after.
// also join consecutive whitespaces, because that's what Bash does
// for the AST BUT NOT WHEN IN QUOTES

void	join_token_pair(t_data *data, t_pre_token *a, t_token_type new_type)
{
	t_pre_token	*b;
	char		*joined;

	if (!a || !a->next)
		return ;
	b = a->next;
	joined = g_malloc(data->hot_zone, sizeof(char) * 3);
	if (!joined)
		return ;
	joined[0] = a->character;
	joined[1] = b->character;
	joined[2] = '\0';
	a->type = new_type;
	a->str = joined;
	a->next = b->next;
	if (b->next)
		b->next->prev = a;
}

bool	should_join_pair(char c1, char c2, t_token_type *out_type)
{
	if (c1 == '<' && c2 == '<')
		*out_type = TOKEN_HEREDOC;
	else if (c1 == '>' && c2 == '>')
		*out_type = TOKEN_APPEND;
	else if (c1 == '&' && c2 == '&')
		*out_type = TOKEN_LOGICAL_AND;
	else if (c1 == '|' && c2 == '|')
		*out_type = TOKEN_LOGICAL_OR;
	else if (c1 == '$' && c2 == '?')
		*out_type = TOKEN_EXIT_STATUS;
	else
		return (false);
	return (true);
}

void	join_double_char_tokens(t_data *data, t_pre_token *head)
{
	t_pre_token		*curr;
	t_token_type	new_type;

	curr = head;
	while (curr && curr->next)
	{
		if (should_join_pair(curr->character, curr->next->character, &new_type))
			join_token_pair(data, curr, new_type);
		else
			curr = curr->next;
	}
}

bool	is_double_char_token(t_token_type type)
{
	if (type == TOKEN_LOGICAL_AND)
		return (true);
	if (type == TOKEN_LOGICAL_OR)
		return (true);
	if (type == TOKEN_HEREDOC)
		return (true);
	if (type == TOKEN_APPEND)
		return (true);
	if (type == TOKEN_EXIT_STATUS)
		return (true);
	return (false);
}

bool	is_metacharacter(t_pre_token *tok)
{
	if (tok->in_squote || tok->in_dquote)
		return (false);
	return (
		tok->type == TOKEN_PIPE
		|| tok->type == TOKEN_BAR
		|| tok->type == TOKEN_LOGICAL_AND
		|| tok->type == TOKEN_LOGICAL_OR
		|| tok->type == TOKEN_LESS
		|| tok->type == TOKEN_GREATER
		|| tok->type == TOKEN_HEREDOC
		|| tok->type == TOKEN_APPEND
		|| tok->type == TOKEN_L_PARENTHESIS
		|| tok->type == TOKEN_R_PARENTHESIS
	);
}
