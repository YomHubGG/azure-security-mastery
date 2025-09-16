/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_grouping_2.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 11:24:31 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:15:16 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	init_word_flags(t_word_flags *flags)
{
	flags->has_var = false;
	flags->has_wild = false;
	flags->has_tilde = false;
}

static void	update_word_flags(t_pre_token *cur, t_word_flags *flags)
{
	if (cur->type == TOKEN_ASTERISK)
		flags->has_wild = true;
	else if (cur->type == TOKEN_DOLLAR || cur->type == TOKEN_EXIT_STATUS)
		flags->has_var = true;
	else if (cur->type == TOKEN_TILDE && !cur->in_squote && !cur->in_dquote)
		flags->has_tilde = true;
}

static void	set_word_flags(t_token *token, t_word_flags *flags)
{
	if (flags->has_var || flags->has_wild || flags->has_tilde)
	{
		if (flags->has_wild)
			token->type = TOKEN_WILDCARD;
		else if (flags->has_var)
			token->type = TOKEN_EXPAND_VARIABLE;
		else if (flags->has_tilde)
			token->type = TOKEN_TILDE_EXPANSION;
	}
}

void	analyze_word_flags(t_pre_token *start, t_pre_token *end,
		t_word_flags *flags)
{
	t_pre_token	*cur;

	cur = start;
	init_word_flags(flags);
	while (cur && cur != end)
	{
		update_word_flags(cur, flags);
		cur = cur->next;
	}
}

t_token	*create_word_token_with_flags(t_data *data, t_pre_token *start,
		t_pre_token *end, t_word_flags *flags)
{
	char	*word;
	t_token	*token;

	word = pre_tokens_to_str(data, start, end);
	if (!word)
		return (NULL);
	token = new_token(TOKEN_WORD, word, data);
	if (!token)
		return (NULL);
	set_word_flags(token, flags);
	if (word && (ft_strchr(word, '\'') || ft_strchr(word, '"')))
	{
		token->quoted = 1;
		set_quote_flags_from_word(token, word);
	}
	return (token);
}
