/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_clear_quote.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/04 15:26:10 by tritter           #+#    #+#             */
/*   Updated: 2025/08/06 13:32:42 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exec_clear_quote(t_data *data, t_tree_node *ast)
{
	t_tree_node	*curr;

	curr = ast;
	while (curr)
	{
		single_node_clear(data, curr);
		curr = curr->token.word->next_word;
	}
}

void	single_node_clear(t_data *data, t_tree_node *ast)
{
	t_snc	acc;

	acc = init_snc();
	acc.to_clear = ast->token.word->word_subtype->value;
	acc.ret = g_malloc(data->hot_zone, ft_strlen(acc.to_clear) + 1);
	if (!acc.ret)
		return (g_arena_clean(data->g_arena), exit (1));
	while (acc.to_clear[acc.i])
	{
		if (acc.to_clear[acc.i] == '\'' && !acc.d_quoted)
		{
			acc.s_quoted = !acc.s_quoted;
			acc.i++;
		}
		else if (acc.to_clear[acc.i] == '\"' && !acc.s_quoted)
		{
			acc.d_quoted = !acc.d_quoted;
			acc.i++;
		}
		else
			acc.ret[acc.i2++] = acc.to_clear[acc.i++];
	}
	acc.ret[acc.i2] = '\0';
	ast->token.word->word_subtype->value = ft_strdup(acc.ret);
	insert_malloc(data->hot_zone, ast->token.word->word_subtype->value);
}

t_snc	init_snc(void)
{
	t_snc	acc;

	acc.i = 0;
	acc.i2 = 0;
	acc.d_quoted = false;
	acc.s_quoted = false;
	return (acc);
}
