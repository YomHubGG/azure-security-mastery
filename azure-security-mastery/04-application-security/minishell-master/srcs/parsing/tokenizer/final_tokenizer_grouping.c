/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_grouping.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:38:11 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 15:47:00 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_word_sequence(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_pre_token		*start;
	t_pre_token		*end;
	t_token			*token;
	t_word_flags	flags;

	start = *cur;
	while (*cur && !is_metacharacter(*cur) && !((*cur)->type == TOKEN_SPACE
			&& !(*cur)->in_squote && !(*cur)->in_dquote))
		*cur = (*cur)->next;
	end = *cur;
	analyze_word_flags(start, end, &flags);
	token = create_word_token_with_flags(data, start, end, &flags);
	if (!token)
		return ;
	append_token(token_head, token_tail, token);
}
