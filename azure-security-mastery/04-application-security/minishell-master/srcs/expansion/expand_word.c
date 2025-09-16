/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 14:44:35 by tritter           #+#    #+#             */
/*   Updated: 2025/08/05 13:23:36 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*hardcode_tilde_expansion(char *word, t_data *data)
{
	char	*home;
	char	*result;

	(void)data;
	if (!word || word[0] != '~')
		return (word);
	if (ft_strcmp(word, "~") == 0)
	{
		home = getenv("HOME");
		if (!home)
			home = "/home";
		result = ft_strdup(home);
		insert_malloc(data->hot_zone, result);
		return (result);
	}
	if (word[1] == '/')
	{
		home = getenv("HOME");
		if (!home)
			home = "/home";
		result = ft_strjoin(home, &word[1]);
		insert_malloc(data->hot_zone, result);
		return (result);
	}
	return (word);
}

int	init_expand_word(t_tree_node *node, t_data *data)
{
	t_init_expand_word	access;

	access.index = 0;
	access.word = &node->token.word->word_subtype->value;
	access.s_quoted = false;
	access.d_quoted = false;
	(*access.word) = hardcode_tilde_expansion((*access.word), data);
	return (expand_validate(access, data));
}

int	expand_validate(t_init_expand_word	access, t_data *data)
{
	while ((*access.word)[access.index])
	{
		if ((*access.word)[access.index] == '\'' && !access.d_quoted)
			access.s_quoted = !access.s_quoted;
		if ((*access.word)[access.index] == '\"' && !access.s_quoted)
			access.d_quoted = !access.d_quoted;
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

char	*expand_word(int *i, char **to_ex, t_data *data)
{
	t_expand_word	access;

	access.fdtok = 0;
	access.end = "";
	access.ret = NULL;
	access.i2 = *i;
	access.begining = ft_substr_injector((*to_ex), 0, *i, data->hot_zone);
	while ((*to_ex)[access.i2])
	{
		if (!expand_word_helper(to_ex, data, &access))
			break ;
	}
	access.end = ft_substr_injector((*to_ex), access.i2,
			ft_strlen((*to_ex)), data->hot_zone);
	if (!access.inbetween)
		return (NULL);
	return (expand_word_join(access, i, data));
}

char	*expand_word_join(t_expand_word access, int *i, t_data *data)
{
	if (ft_strcmp(access.inbetween, "?") == 0)
	{
		access.inbetween = ft_itoa(data->last_exit_status);
		insert_malloc(data->hot_zone, access.inbetween);
	}
	else
		access.inbetween = extract_word(access.inbetween, data);
	access.ret = "";
	access.ret = ft_strjoin(access.ret, access.begining);
	if (!access.ret)
		return (NULL);
	insert_malloc(data->hot_zone, access.ret);
	access.ret = ft_strjoin(access.ret, access.inbetween);
	if (!access.ret)
		return (NULL);
	insert_malloc(data->hot_zone, access.ret);
	*i = ft_strlen(access.ret);
	access.ret = ft_strjoin(access.ret, access.end);
	if (!access.ret)
		return (NULL);
	insert_malloc(data->hot_zone, access.ret);
	return (access.ret);
}
