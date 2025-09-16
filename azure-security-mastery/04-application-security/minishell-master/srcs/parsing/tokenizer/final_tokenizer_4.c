/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_4.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:12:55 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 15:46:15 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_heredoc_del(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*delimiter;

	delimiter = collect_heredoc_delimiter(cur, data, false);
	append_token(token_head, token_tail, delimiter);
}

void	advance_to_closing_quote(t_pre_token **cur, t_token_type quote_type)
{
	while (*cur && (*cur)->type != quote_type)
		*cur = (*cur)->next;
}

t_token	*collect_heredoc_delimiter(t_pre_token **cur, t_data *data,
		bool is_quoted)
{
	t_pre_token	*start;
	t_pre_token	*end;
	char		*word;
	bool		contains_quotes;

	contains_quotes = delimiter_contains_quotes(*cur);
	if (is_quoted && ((*cur)->type == TOKEN_SINGLE_QUOTE
			|| (*cur)->type == TOKEN_DOUBLE_QUOTE))
		process_quoted_delimiter(cur, &start, &end);
	else
		process_unquoted_delimiter(cur, &start, &end);
	word = pre_tokens_to_str(data, start, end);
	if (!word)
		return (NULL);
	return (create_delimiter_token(data, word, (is_quoted || contains_quotes),
			cur));
}

void	handle_quote_heredoc_del(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*delimiter;

	delimiter = collect_heredoc_delimiter(cur, data, true);
	if (!delimiter)
		return ;
	append_token(token_head, token_tail, delimiter);
}

void	set_heredoc_token_quoted_attrs(t_token *token, bool is_single_quote)
{
	token->quoted = 1;
	token->fully_quoted = 1;
	if (is_single_quote)
		token->quote_type = 1;
	else
		token->quote_type = 2;
}
