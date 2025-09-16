/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   early_error_check_5.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:45:01 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 15:45:21 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static t_syntax_error	*has_early_syntax_error_2(t_data *data)
{
	if (has_unexpected_tokens(data) && !data->set_syntax_error->has_error)
		return (data->set_syntax_error);
	if (has_empty_command_between(data) && !data->set_syntax_error->has_error)
		return (data->set_syntax_error);
	if (has_invalid_redirections(data))
		return (data->set_syntax_error);
	return (data->set_syntax_error);
}

t_syntax_error	*has_early_syntax_error(t_data *data)
{
	if (has_consecutive_separators(data))
		return (data->set_syntax_error);
	if (!quotes_are_closed(data->pre_token_head, data)
		&& !data->set_syntax_error->has_error)
		return (data->set_syntax_error);
	if (!parenthesis_is_valid(data->pre_token_head)
		&& !data->set_syntax_error->has_error)
	{
		data->set_syntax_error->has_error = true;
		data->set_syntax_error->message = ")";
		return (data->set_syntax_error);
	}
	if (has_empty_parentheses(data, data->pre_token_head)
		&& !data->set_syntax_error->has_error)
	{
		data->set_syntax_error->has_error = true;
		data->set_syntax_error->message = ")";
		return (data->set_syntax_error);
	}
	return (has_early_syntax_error_2(data));
}

bool	set_syntax_error(t_data *data, t_pre_token *token, const char *message)
{
	if (!data->set_syntax_error)
		return (0);
	data->set_syntax_error->has_error = true;
	data->set_syntax_error->message = message;
	data->set_syntax_error->token = token;
	return (1);
}
