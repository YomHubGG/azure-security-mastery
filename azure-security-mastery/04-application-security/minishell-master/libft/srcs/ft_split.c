/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 12:27:56 by ggrisole          #+#    #+#             */
/*   Updated: 2024/11/09 10:45:29 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	**ft_freestring(char **res, size_t x)
{
	while (x > 0)
	{
		free(res[x - 1]);
		x--;
	}
	free(res);
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

static char	**ft_allocate_and_fill(char const *s, char c)
{
	char	**res;
	size_t	i;
	int		x;

	res = ft_calloc((ft_countwords(s, c) + 1), sizeof(char *));
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
			x++;
		}
		else
			i++;
	}
	res[x] = NULL;
	return (res);
}

char	**ft_split(char const *s, char c)
{
	if (!s)
		return (NULL);
	return (ft_allocate_and_fill(s, c));
}

// int	main(void)
// {
// 	/*
// 	char **string = ft_split("le petit poussait", ' ');
// 	for(int i = 0; string[i]; i++)
// 		printf("%s\n", string[i]);

// 	printf("%d", ft_countwords(" mes amis non non ", ' '));*/
// }