/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 18:44:27 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:16:10 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_word_token(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*word;

	word = collect_word_token(cur, data);
	append_token(token_head, token_tail, word);
}

void	handle_expand_var_token(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*var;

	var = collect_expand_var_token(cur, data);
	append_token(token_head, token_tail, var);
}

void	handle_single_token(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*op;

	op = create_single_token(*cur, data);
	if (op && op->type == TOKEN_BAR)
		op->type = TOKEN_PIPE;
	append_token(token_head, token_tail, op);
	*cur = (*cur)->next;
}

static void	group_token_dispatch(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	if (*token_tail && (*token_tail)->type == TOKEN_HEREDOC)
	{
		if ((*cur)->type == TOKEN_SINGLE_QUOTE
			|| (*cur)->type == TOKEN_DOUBLE_QUOTE)
			return (handle_quote_heredoc_del(token_head, token_tail, cur,
					data));
		else if (delimiter_contains_quotes(*cur))
			return (handle_heredoc_del_with_embedded_quotes(token_head,
					token_tail, cur, data));
		else
			return (handle_heredoc_del(token_head, token_tail, cur, data));
	}
	if ((*cur)->type == TOKEN_TILDE && !(*cur)->in_squote && !(*cur)->in_dquote)
		return (handle_tilde_token(token_head, token_tail, cur, data));
	if (!is_metacharacter(*cur) && (*cur)->type != TOKEN_SPACE)
		return (handle_word_sequence(token_head, token_tail, cur, data));
	handle_single_token(token_head, token_tail, cur, data);
}

t_token	*group_pre_tokens(t_pre_token *pre_head, t_data *data)
{
	t_token		*token_head;
	t_token		*token_tail;
	t_pre_token	*cur;

	token_head = NULL;
	token_tail = NULL;
	cur = pre_head;
	while (cur)
	{
		cur = skip_spaces(cur);
		if (!cur)
			break ;
		group_token_dispatch(&token_head, &token_tail, &cur, data);
	}
	return (token_head);
}
