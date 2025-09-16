/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_4.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/11 12:22:59 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/21 13:08:01 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_tokenize_helper	*init_tokenize_helper(t_data *data)
{
	t_tokenize_helper	*init;

	init = g_malloc(data->hot_zone, sizeof(t_tokenize_helper));
	if (!init)
		return (NULL);
	init->in_squote = false;
	init->in_dquote = false;
	init->i = 0;
	init->head = create_pre_token_node(data);
	if (!init->head)
		return (NULL);
	init->current = init->head;
	return (init);
}

void	secure_add_pre_token_node(t_data *data, t_tokenize_helper *helper)
{
	helper->current = add_pre_token_node(data, helper->current);
	if (!helper->current)
		return ;
	helper->current->in_squote = helper->in_squote;
	helper->current->in_dquote = helper->in_dquote;
}
