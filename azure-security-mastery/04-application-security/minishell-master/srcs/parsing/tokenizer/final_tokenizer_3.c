/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:11:38 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:14:05 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	add_char_to_str(char *str, int *i, t_pre_token *cur)
{
	if (cur->type == TOKEN_EXIT_STATUS)
	{
		str[(*i)++] = '$';
		str[(*i)++] = '?';
	}
	else
		str[(*i)++] = cur->character;
}

static int	calculate_token_str_len(t_pre_token *start, t_pre_token *end)
{
	int			len;
	t_pre_token	*cur;

	len = 0;
	cur = start;
	while (cur && cur != end)
	{
		if (cur->type == TOKEN_EXIT_STATUS)
			len += 2;
		else
			len++;
		cur = cur->next;
	}
	return (len);
}

char	*pre_tokens_to_str(t_data *data, t_pre_token *start, t_pre_token *end)
{
	int			len;
	t_pre_token	*cur;
	char		*str;
	int			i;

	len = calculate_token_str_len(start, end);
	str = g_malloc(data->hot_zone, len + 1);
	if (!str)
		return (NULL);
	cur = start;
	i = 0;
	while (cur && cur != end)
	{
		add_char_to_str(str, &i, cur);
		cur = cur->next;
	}
	str[i] = '\0';
	return (str);
}

t_token	*collect_single_quoted_token(t_pre_token **cur, t_data *data)
{
	t_pre_token	*start;
	t_pre_token	*end;
	char		*value;
	t_token		*tok;

	start = *cur;
	*cur = (*cur)->next;
	end = *cur;
	while (*cur && !((*cur)->type == TOKEN_SINGLE_QUOTE
			&& (*cur)->in_squote == 0))
		*cur = (*cur)->next;
	if (*cur && (*cur)->type == TOKEN_SINGLE_QUOTE)
	{
		end = *cur;
		*cur = (*cur)->next;
	}
	value = pre_tokens_to_str(data, start, end->next);
	tok = new_token(TOKEN_WORD, value, data);
	tok->quoted = 1;
	tok->fully_quoted = 1;
	tok->quote_type = 1;
	return (tok);
}

t_token	*collect_double_quoted_token(t_pre_token **cur, t_data *data)
{
	t_pre_token	*start;
	t_pre_token	*end;
	char		*value;
	t_token		*tok;

	start = *cur;
	*cur = (*cur)->next;
	end = *cur;
	while (*cur && !((*cur)->type == TOKEN_DOUBLE_QUOTE
			&& (*cur)->in_dquote == 0))
		*cur = (*cur)->next;
	if (*cur && (*cur)->type == TOKEN_DOUBLE_QUOTE)
	{
		end = *cur;
		*cur = (*cur)->next;
	}
	value = pre_tokens_to_str(data, start, end->next);
	tok = new_token(TOKEN_WORD, value, data);
	tok->quoted = 1;
	tok->fully_quoted = 1;
	tok->quote_type = 2;
	return (tok);
}
