/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_wildcard.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:25:44 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:15:47 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_var_char(char c)
{
	return (ft_isalnum(c) || c == '_');
}

t_token	*new_token(t_token_type type, char *value, t_data *data)
{
	t_token	*tok;

	tok = g_malloc(data->hot_zone, sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->type = type;
	tok->value = value;
	tok->next = NULL;
	tok->prev = NULL;
	tok->quoted = 0;
	tok->fully_quoted = 0;
	tok->joined = 0;
	tok->quote_type = 0;
	return (tok);
}

void	handle_tilde_token(t_token **token_head, t_token **token_tail,
		t_pre_token **cur, t_data *data)
{
	t_token	*token;
	char	*value;

	value = gc_strdup(data, "~");
	if (!value)
		return ;
	token = new_token(TOKEN_TILDE_EXPANSION, value, data);
	append_token(token_head, token_tail, token);
	*cur = (*cur)->next;
}
