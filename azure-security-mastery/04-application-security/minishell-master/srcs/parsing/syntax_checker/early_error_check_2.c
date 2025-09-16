/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   early_error_check_2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:22:44 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/27 14:10:14 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	is_separator(t_pre_token *tok)
{
	return (tok->type == TOKEN_PIPE || tok->type == TOKEN_BAR
		|| tok->type == TOKEN_LOGICAL_AND || tok->type == TOKEN_LOGICAL_OR);
}

static bool	is_bad_start(t_pre_token *tok)
{
	while (tok && tok->type == TOKEN_SPACE)
		tok = tok->next;
	return (tok && (is_separator(tok) || tok->type == TOKEN_R_PARENTHESIS));
}

char	*get_error_token_str(t_pre_token *tok)
{
	if (!tok)
		return ("newline");
	return (get_token_char_for_error(tok->type));
}

bool	is_bad_sequence(t_pre_token *tok)
{
	t_pre_token	*next;

	if (!is_separator(tok))
		return (false);
	next = tok->next;
	while (next && next->type == TOKEN_SPACE)
		next = next->next;
	return (!next || is_separator(next) || next->type == TOKEN_R_PARENTHESIS);
}

bool	has_unexpected_tokens(t_data *data)
{
	t_pre_token	*tok;
	t_pre_token	*error_tok;
	t_pre_token	*next;

	next = NULL;
	tok = data->pre_token_head;
	if (!tok)
		return (false);
	if (is_bad_start(tok))
	{
		error_tok = tok;
		while (error_tok && error_tok->type == TOKEN_SPACE)
			error_tok = error_tok->next;
		return (set_syntax_error(data, error_tok,
				get_error_token_str(error_tok)), true);
	}
	return (has_unexpected_tokens_helper(tok, data, next));
}
