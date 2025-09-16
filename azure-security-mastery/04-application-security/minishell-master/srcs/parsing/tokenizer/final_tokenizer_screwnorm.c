/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_screwnorm.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:11:58 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 16:28:40 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_pre_token	*skip_spaces(t_pre_token *cur)
{
	while (cur && cur->type == TOKEN_SPACE)
		cur = cur->next;
	return (cur);
}
