/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   early_error_check_4.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:25:11 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 15:45:11 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	has_empty_parentheses(t_data *data, t_pre_token *head)
{
	while (head && head->next)
	{
		if (head->type == TOKEN_L_PARENTHESIS
			&& head->next->type == TOKEN_R_PARENTHESIS)
		{
			set_syntax_error(data, head->next, ")");
			return (true);
		}
		head = head->next;
	}
	return (false);
}

bool	has_consecutive_separators(t_data *data)
{
	t_pre_token	*cur;
	t_pre_token	*next_non_space;

	cur = data->pre_token_head;
	while (cur && cur->next)
	{
		if (is_separator(cur))
		{
			next_non_space = cur->next;
			while (next_non_space && next_non_space->type == TOKEN_SPACE)
				next_non_space = next_non_space->next;
			if (next_non_space && is_separator(next_non_space))
			{
				set_syntax_error(data, next_non_space,
					get_token_char_for_error(next_non_space->type));
				return (true);
			}
		}
		cur = cur->next;
	}
	return (false);
}

static bool	is_bad_redirection_target(t_pre_token *tok)
{
	return (tok->type == TOKEN_PIPE || tok->type == TOKEN_LOGICAL_AND
		|| tok->type == TOKEN_LOGICAL_OR || tok->type == TOKEN_LESS
		|| tok->type == TOKEN_GREATER || tok->type == TOKEN_APPEND
		|| tok->type == TOKEN_HEREDOC);
}

bool	has_invalid_redirections(t_data *data)
{
	t_pre_token	*tok;
	t_pre_token	*next;

	tok = data->pre_token_head;
	while (tok)
	{
		if (tok->type == TOKEN_LESS || tok->type == TOKEN_GREATER
			|| tok->type == TOKEN_APPEND || tok->type == TOKEN_HEREDOC)
		{
			next = tok->next;
			while (next && next->type == TOKEN_SPACE)
				next = next->next;
			if (!next)
				return (set_syntax_error(data, tok,
						"newline"), true);
			if (is_bad_redirection_target(next))
				return (set_syntax_error(data, next,
						get_token_char_for_error(next->type)), true);
		}
		tok = tok->next;
	}
	return (false);
}
