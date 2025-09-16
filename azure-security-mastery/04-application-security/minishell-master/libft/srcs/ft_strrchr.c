/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 15:23:04 by ggrisole          #+#    #+#             */
/*   Updated: 2024/10/28 21:28:40 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int		i;
	char	j;

	j = (char)c;
	i = ft_strlen(s);
	while (i >= 0 && s[i] != j)
		i--;
	if (j == s[i])
		return ((char *)&s[i]);
	return (NULL);
}
/*
int	main(void)
{
	printf("%s\n", ft_strrchr("destiny", 115));
}
*/