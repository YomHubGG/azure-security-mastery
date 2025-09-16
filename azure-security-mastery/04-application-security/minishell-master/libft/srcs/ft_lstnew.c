/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:42:32 by ggrisole          #+#    #+#             */
/*   Updated: 2024/11/09 11:04:19 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*teliste;

	teliste = malloc(sizeof(t_list));
	if (!teliste)
		return (NULL);
	teliste->content = content;
	teliste->next = NULL;
	return (teliste);
}
