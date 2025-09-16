/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 15:08:28 by ggrisole          #+#    #+#             */
/*   Updated: 2025/05/27 22:34:36 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_pre_token	*sanitize_pre_tokens(t_data *data, t_pre_token *head)
{
	join_double_char_tokens(data, head);
	head = trim_token_whitespace(head);
	compress_spaces(&head);
	return (head);
}

t_pre_token	*trim_token_whitespace(t_pre_token *head)
{
	t_pre_token	*tmp;

	while (head && head->type == TOKEN_SPACE)
	{
		tmp = head;
		head = head->next;
		if (head)
			head->prev = NULL;
	}
	if (!head)
		return (NULL);
	tmp = head;
	while (tmp->next)
		tmp = tmp->next;
	while (tmp && tmp->type == TOKEN_SPACE)
	{
		tmp = tmp->prev;
		if (tmp)
			tmp->next = NULL;
	}
	return (head);
}

int	is_quote_open(char c, int *s_quote, int *d_quote)
{
	if (c == '\'' && *d_quote == 0)
		*s_quote = !(*s_quote);
	else if (c == '\"' && *s_quote == 0)
		*d_quote = !(*d_quote);
	if (*s_quote || *d_quote)
		return (1);
	return (0);
}

void	compress_spaces(t_pre_token **head)
{
	t_pre_token	*cur;
	t_pre_token	*to_remove;

	cur = *head;
	while (cur)
	{
		if (cur->type == TOKEN_SPACE && !cur->in_squote && !cur->in_dquote)
		{
			while (cur->next && cur->next->type == TOKEN_SPACE)
			{
				to_remove = cur->next;
				cur->next = to_remove->next;
				if (to_remove->next)
					to_remove->next->prev = cur;
			}
		}
		cur = cur->next;
	}
}

char	safe_check_next(char *str)
{
	if (!str)
		return ('\0');
	if (*str == '\0')
		return ('\0');
	if (*(str + 1) == '\0')
		return ('\0');
	return (*(str + 1));
}
