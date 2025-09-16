/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_split.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 14:01:49 by tritter           #+#    #+#             */
/*   Updated: 2025/05/29 15:21:15 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.h"
#include "../includes/garbage.h"

static char	**ft_freestring(char **res, size_t x)
{
	while (x > 0)
	{
		free(res[x]);
		res[x] = NULL;
		x--;
	}
	free(res);
	res = NULL;
	return (NULL);
}

static int	ft_countwords(char const *s, char c)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	if (s[0] != c && s[0] != '\0')
		j++;
	while (s[i + 1])
	{
		if (s[i] == c && s[i + 1] != c)
			j++;
		i++;
	}
	return (j);
}

static char	*ft_getword(char const *s, char c, size_t *i)
{
	unsigned int	start;

	while (s[*i] == c && s[*i] != '\0')
		(*i)++;
	start = *i;
	while (s[*i] != c && s[*i] != '\0')
		(*i)++;
	return (ft_substr(s, start, *i - start));
}

static char	**ft_allocate_and_fill(t_zone_g **zone, char const *s, char c)
{
	char	**res;
	size_t	i;
	int		x;

	res = ft_g_calloc(*zone, (ft_countwords(s, c) + 1), sizeof(char *));
	if (!res)
		return (NULL);
	i = 0;
	x = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			res[x] = ft_getword(s, c, &i);
			if (!res[x])
				return (ft_freestring(res, x));
			insert_malloc(zone, res[x]);
			x++;
		}
		else
			i++;
	}
	res[x] = NULL;
	return (res);
}

char	**ft_g_split(t_zone_g **zone, char const *s, char c)
{
	if (!s)
		return (NULL);
	return (ft_allocate_and_fill(zone, s, c));
}
