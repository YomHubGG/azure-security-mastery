/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run_pipeline_wrapper.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 16:15:22 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/17 15:10:51 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	run_tokenization_pipeline(t_data *data, char *input)
{
	t_pre_token		*pre_tokens;
	t_token			*tokens;
	t_syntax_error	*err;

	if (!data || !input)
		return ;
	data->pre_token_head = NULL;
	data->token_head = NULL;
	pre_tokens = tokenize(data, input);
	if (!pre_tokens)
		return ;
	data->pre_token_head = pre_tokens;
	err = has_early_syntax_error(data);
	if (err && err->has_error)
		return (print_syntax_error(data));
	tokens = group_pre_tokens(data->pre_token_head, data);
	if (!tokens)
		return ;
	data->token_head = tokens;
}
