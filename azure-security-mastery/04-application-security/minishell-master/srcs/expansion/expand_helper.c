/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 12:16:49 by tritter           #+#    #+#             */
/*   Updated: 2025/07/17 14:29:18 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//Used to inject malloc value of strjoin inside the specified t_zone_g
char	*ft_strjoin_injector(const char *s1, const char *s2, t_zone_g **zone)
{
	char	*new_str;

	new_str = ft_strjoin(s1, s2);
	if (!new_str)
		return (NULL);
	insert_malloc(zone, new_str);
	return (new_str);
}

char	*ft_itoa_injector(int n, t_zone_g **zone)
{
	char	*new_str;

	new_str = ft_itoa(n);
	if (!new_str)
		return (NULL);
	insert_malloc(zone, new_str);
	return (new_str);
}

char	**ft_split_injector(char const *s, char c, t_zone_g **zone)
{
	char	**new_tab;
	int		i;

	i = 0;
	new_tab = ft_split(s, c);
	if (!new_tab)
		return (NULL);
	while (new_tab[i])
	{
		insert_malloc(zone, new_tab[i]);
		i++;
	}
	insert_malloc(zone, new_tab);
	return (new_tab);
}

char	*ft_substr_injector(const char *s, unsigned int start, size_t len,
	t_zone_g **zone)
{
	char	*sub;

	sub = ft_substr(s, start, len);
	if (!sub)
		return (NULL);
	insert_malloc(zone, sub);
	return (sub);
}

bool	expand_word_helper(char **to_ex, t_data *data,
	t_expand_word *ac)
{
	int	start_pos;

	if ((*to_ex)[(*ac).i2] == '$')
	{
		(*ac).fdtok = (*ac).i2++;
		start_pos = (*ac).i2;
		if ((*to_ex)[(*ac).i2] == '?')
			(*ac).i2++;
		else if (ft_isdigit((*to_ex)[(*ac).i2]))
			(*ac).i2++;
		else
			while (ft_isalnum((*to_ex)[(*ac).i2]) || (*to_ex)[(*ac).i2] == '_')
				(*ac).i2++;
		if ((*ac).i2 == start_pos && (*to_ex)[start_pos] != '?')
			expand_word_helper_subwrp(ac, data);
		else
			(*ac).inbetween = ft_substr_injector((*to_ex), (*ac).fdtok + 1,
					(*ac).i2 - ((*ac).fdtok + 1), data->hot_zone);
		return (false);
	}
	(*ac).i2++;
	return (true);
}
