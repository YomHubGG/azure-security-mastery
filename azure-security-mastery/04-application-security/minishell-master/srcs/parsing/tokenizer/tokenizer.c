/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:10:36 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/17 15:11:27 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	handle_escaped_in_dquote(const char *input,
		t_tokenize_helper *helper)
{
	char	next;

	if (helper->in_dquote && input[helper->i] == '\\')
	{
		next = input[helper->i + 1];
		if (next == '"' || next == '$' || next == '\\' || next == '`')
		{
			helper->current->character = next;
			helper->current->type = TOKEN_CHAR;
			helper->current->in_squote = helper->in_squote;
			helper->current->in_dquote = helper->in_dquote;
			return (2);
		}
	}
	return (0);
}

static void	handle_quote_state(t_tokenize_helper *helper, char current)
{
	if (current == '\'' && !helper->in_dquote)
	{
		helper->in_squote = !helper->in_squote;
		helper->current->in_squote = helper->in_squote;
	}
	else if (current == '"' && !helper->in_squote)
	{
		helper->in_dquote = !helper->in_dquote;
		helper->current->in_dquote = helper->in_dquote;
	}
}

static int	process_current_char(const char *input, t_tokenize_helper *helper,
		t_data *data)
{
	helper->consumed = handle_escaped_in_dquote(input, helper);
	if (helper->consumed)
		return (helper->consumed);
	handle_quote_state(helper, input[helper->i]);
	helper->new_input = process_token((char *)&input[helper->i], data,
			helper->current);
	return (helper->new_input - input - helper->i);
}

t_pre_token	*tokenize(t_data *data, const char *input)
{
	t_tokenize_helper	*helper;
	int					advance;

	helper = init_tokenize_helper(data);
	if (!helper)
		return (NULL);
	while (input[helper->i])
	{
		advance = process_current_char(input, helper, data);
		helper->i += advance;
		if (input[helper->i])
			secure_add_pre_token_node(data, helper);
		if (!helper->current)
			return (NULL);
	}
	return (helper->head);
}

char	*process_token(char *prompt, t_data *data, t_pre_token *node)
{
	char			current;
	char			next;
	t_token_type	type;

	(void)data;
	current = *prompt;
	next = safe_check_next(prompt);
	type = pre_tokenize(current, next);
	if (type == TOKEN_ASTERISK && (node->in_squote || node->in_dquote))
		type = TOKEN_CHAR;
	node->character = current;
	node->type = type;
	if (is_double_char_token(type) && next != '\0')
		prompt++;
	return (prompt + 1);
}
