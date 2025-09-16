/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_5.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:23:36 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:14:37 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	collect_delimiter_content(t_pre_token **cur, bool is_quoted)
{
	if (is_quoted)
		return ;
	else
	{
		while (*cur && (*cur)->type != TOKEN_SPACE && !is_metacharacter(*cur))
			*cur = (*cur)->next;
	}
}

void	finalize_heredoc_token(t_token *token, t_pre_token **cur,
		bool is_quoted)
{
	if (is_quoted)
	{
		token->quoted = 1;
		token->fully_quoted = 1;
		if ((*cur) && (*cur)->prev && (*cur)->prev->type == TOKEN_DOUBLE_QUOTE)
			token->quote_type = 2;
		else if ((*cur) && (*cur)->prev
			&& (*cur)->prev->type == TOKEN_SINGLE_QUOTE)
			token->quote_type = 1;
	}
}

void	process_quoted_delimiter(t_pre_token **cur, t_pre_token **start,
		t_pre_token **end)
{
	t_token_type	quote_type;

	quote_type = (*cur)->type;
	*cur = (*cur)->next;
	*start = *cur;
	while (*cur && (*cur)->type != quote_type)
		*cur = (*cur)->next;
	*end = *cur;
	if (*cur)
		*cur = (*cur)->next;
}

void	process_unquoted_delimiter(t_pre_token **cur, t_pre_token **start,
		t_pre_token **end)
{
	*start = *cur;
	collect_delimiter_content(cur, false);
	*end = *cur;
}

t_token	*create_delimiter_token(t_data *data, char *word, bool is_quoted,
		t_pre_token **cur)
{
	t_token			*token;
	t_token_type	type;

	if (is_quoted)
		type = TOKEN_HEREDOC_DELIM_QUOTED;
	else
		type = TOKEN_HEREDOC_DELIM;
	token = new_token(type, word, data);
	if (!token)
		return (NULL);
	finalize_heredoc_token(token, cur, is_quoted);
	return (token);
}
