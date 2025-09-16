/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc_expand.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/23 17:16:53 by tritter           #+#    #+#             */
/*   Updated: 2025/07/27 12:48:00 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	hd_init_expand_word(char **to_ex, t_data *data)
{
	t_init_expand_word	access;

	access.index = 0;
	access.word = to_ex;
	access.s_quoted = false;
	access.d_quoted = false;
	return (hd_expand_validate(access, data));
}

char	*hd_quote_clear(t_data *data, char *to_clear)
{
	t_snc	acc;

	acc = init_snc();
	acc.to_clear = to_clear;
	acc.ret = g_malloc(data->hot_zone, ft_strlen(acc.to_clear) + 1);
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
	return (acc.ret);
}

int	hd_expand_validate(t_init_expand_word	access, t_data *data)
{
	while ((*access.word)[access.index])
	{
		if ((*access.word)[access.index] == '$' && access.s_quoted == false)
		{
			if (!(*access.word)[access.index + 1]
				|| (!ft_isalnum((*access.word)[access.index + 1])
					&& (*access.word)[access.index + 1] != '?'))
				access.index++;
			else
			{
				(*access.word) = expand_word(&access.index, access.word, data);
				if (!access.word)
					return (EXPAND_MALLOC_ERR);
			}
		}
		else
			access.index++;
	}
	return (0);
}
