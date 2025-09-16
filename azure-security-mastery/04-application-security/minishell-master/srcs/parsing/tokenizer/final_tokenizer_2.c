/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 10:27:31 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 16:27:45 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_token	*collect_word_token(t_pre_token **cur, t_data *data)
{
	t_pre_token	*start;
	t_pre_token	*s;
	bool		has_quotes;
	char		*word;
	t_token		*token;

	start = *cur;
	s = *cur;
	has_quotes = false;
	while (s && !is_metacharacter(s) && !(s->type == TOKEN_SPACE
			&& !s->in_squote && !s->in_dquote))
	{
		if (s->type == TOKEN_SINGLE_QUOTE || s->type == TOKEN_DOUBLE_QUOTE)
			has_quotes = true;
		s = s->next;
	}
	*cur = s;
	word = pre_tokens_to_str(data, start, s);
	token = new_token(TOKEN_WORD, word, data);
	if (has_quotes && token)
	{
		token->quoted = 1;
		set_quote_flags_from_word(token, word);
	}
	return (token);
}

t_token	*collect_expand_var_token(t_pre_token **cur, t_data *data)
{
	t_pre_token	*start;
	char		*var;

	start = *cur;
	*cur = (*cur)->next;
	while (*cur && is_var_char((*cur)->character))
		*cur = (*cur)->next;
	var = pre_tokens_to_str(data, start, *cur);
	return (new_token(TOKEN_EXPAND_VARIABLE, var, data));
}

t_token	*create_single_token(t_pre_token *cur, t_data *data)
{
	char			*value;
	t_token_type	final_type;
	char			str[2];

	if (cur->type == TOKEN_HEREDOC && cur->str)
		value = gc_strdup(data, cur->str);
	else
	{
		str[0] = cur->character;
		str[1] = '\0';
		value = gc_strdup(data, str);
	}
	if (!value)
		return (NULL);
	final_type = cur->type;
	if (final_type == TOKEN_BAR)
		final_type = TOKEN_PIPE;
	else if (final_type == TOKEN_LOGICAL_AND)
		final_type = TOKEN_LOGICAL_AND;
	else if (final_type == TOKEN_LOGICAL_AND)
		final_type = TOKEN_LOGICAL_AND;
	return (new_token(final_type, value, data));
}

void	append_token(t_token **head, t_token **tail, t_token *new_token)
{
	if (!new_token)
		return ;
	new_token->next = NULL;
	if (!*head)
	{
		*head = new_token;
		*tail = new_token;
		new_token->prev = NULL;
	}
	else
	{
		(*tail)->next = new_token;
		new_token->prev = *tail;
		*tail = new_token;
	}
}

int	is_var_start(char c)
{
	return (ft_isalpha(c) || c == '_');
}
