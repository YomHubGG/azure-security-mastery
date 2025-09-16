/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   early_error_check_3.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:24:26 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/17 15:13:41 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	has_empty_command_between(t_data *data)
{
	t_pre_token	*token;
	bool		prev_sep;

	token = data->pre_token_head;
	prev_sep = false;
	while (token)
	{
		if (!token->in_squote && !token->in_dquote)
		{
			if (handle_separator_state(data, &prev_sep, token))
				return (true);
		}
		token = token->next;
	}
	return (false);
}

bool	handle_separator_state(t_data *data, bool *prev_sep, t_pre_token *token)
{
	if (token->type == TOKEN_SPACE)
		return (false);
	if (is_separator(token))
	{
		if (*prev_sep)
		{
			set_syntax_error(data, token,
				get_token_char_for_error(token->type));
			return (true);
		}
		*prev_sep = true;
	}
	else
		*prev_sep = false;
	return (false);
}
