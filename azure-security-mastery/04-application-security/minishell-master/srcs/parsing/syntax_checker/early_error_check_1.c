/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   early_error_check_1.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:13:34 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/27 13:57:48 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	has_unexpected_tokens_helper(t_pre_token *tok, t_data *data,
	t_pre_token *next)
{
	while (tok)
	{
		if (is_bad_sequence(tok))
		{
			next = tok->next;
			while (next && next->type == TOKEN_SPACE)
				next = next->next;
			return (set_syntax_error(data, next,
					get_error_token_str(next)), true);
		}
		tok = tok->next;
	}
	return (false);
}

bool	update_paren_counter(t_pre_token *tok, int *count)
{
	if (tok->type == TOKEN_L_PARENTHESIS)
		(*count)++;
	else if (tok->type == TOKEN_R_PARENTHESIS)
	{
		(*count)--;
		if (*count < 0)
			return (false);
	}
	return (true);
}

bool	parenthesis_is_valid(t_pre_token *head)
{
	int	count;

	count = 0;
	while (head)
	{
		if (!head->in_squote && !head->in_dquote)
			if (!update_paren_counter(head, &count))
				return (false);
		head = head->next;
	}
	if (count != 0)
		return (false);
	return (true);
}

bool	quotes_are_closed(t_pre_token *tok, t_data *data)
{
	t_pre_token	*last;

	last = NULL;
	while (tok)
	{
		last = tok;
		tok = tok->next;
	}
	if (last && last->in_squote)
	{
		set_syntax_error(data, last, "'");
		return (false);
	}
	if (last && last->in_dquote)
	{
		set_syntax_error(data, last, "\"");
		return (false);
	}
	return (true);
}
