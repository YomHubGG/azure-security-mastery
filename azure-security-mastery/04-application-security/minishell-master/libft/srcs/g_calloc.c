/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   g_calloc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 14:04:24 by tritter           #+#    #+#             */
/*   Updated: 2025/06/11 13:56:05 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/garbage.h"

void	*ft_g_calloc(t_zone_g *zone, size_t nmemb, size_t size)
{
	char	*ptr;
	size_t	i;
	size_t	res;

	if (nmemb == 0 || size == 0)
		return (NULL);
	res = nmemb * size;
	if (nmemb != 0 && res / nmemb != size)
		return (NULL);
	i = 0;
	ptr = g_malloc(&zone, nmemb * size);
	if (!ptr)
		return (NULL);
	while (i < nmemb * size)
		ptr[i++] = 0;
	return (ptr);
}
