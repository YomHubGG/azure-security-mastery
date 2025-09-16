/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_tokenizer_6.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:44:08 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/04 16:28:12 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	set_quote_flags_from_word(t_token *token, char *word)
{
	int		len;
	bool	has_single;
	bool	has_double;

	len = ft_strlen(word);
	has_single = ft_strchr(word, '\'') != NULL;
	has_double = ft_strchr(word, '"') != NULL;
	if (has_single && has_double)
		token->quote_type = 3;
	else if (has_single)
		token->quote_type = 1;
	else if (has_double)
		token->quote_type = 2;
	else
		token->quote_type = 0;
	if (len >= 2 && token->quote_type != 3)
	{
		if ((word[0] == '\'' && word[len - 1] == '\'') || (word[0] == '"'
				&& word[len - 1] == '"'))
			token->fully_quoted = 1;
		else
			token->fully_quoted = 0;
	}
	else
		token->fully_quoted = 0;
}

bool	delimiter_contains_quotes(t_pre_token *cur)
{
	while (cur && cur->type != TOKEN_SPACE && !is_separator(cur))
	{
		if (cur->type == TOKEN_SINGLE_QUOTE || cur->type == TOKEN_DOUBLE_QUOTE)
			return (true);
		cur = cur->next;
	}
	return (false);
}

void	handle_heredoc_del_with_embedded_quotes(t_token **token_head,
		t_token **token_tail, t_pre_token **cur, t_data *data)
{
	t_token	*delimiter;

	delimiter = collect_heredoc_delimiter(cur, data, true);
	if (!delimiter)
		return ;
	delimiter->type = TOKEN_HEREDOC_DELIM_QUOTED;
	append_token(token_head, token_tail, delimiter);
}
