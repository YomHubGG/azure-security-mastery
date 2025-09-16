/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:07:48 by ggrisole          #+#    #+#             */
/*   Updated: 2024/11/09 09:12:26 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_nbrlen(int n)
{
	int	i;

	i = 1;
	while (n > 9 || n < -9)
	{
		n = n / 10;
		i++;
	}
	return (i);
}

static char	*ft_process_int_min(void)
{
	char	*res;

	res = ft_strdup("-2147483648");
	if (!res)
		return (NULL);
	return (res);
}

char	*ft_itoa(int n)
{
	char	*res;
	size_t	len;
	size_t	is_negative;

	if (n == -2147483648)
		return (ft_process_int_min());
	is_negative = (n < 0);
	len = ft_nbrlen(n) + is_negative;
	res = (char *)malloc(sizeof(char) * (len + 1));
	if (!res)
		return (NULL);
	if (is_negative)
	{
		n = -n;
		res[0] = '-';
	}
	res[len] = '\0';
	while (len-- > is_negative)
	{
		res[len] = (n % 10) + '0';
		n = n / 10;
	}
	return (res);
}
