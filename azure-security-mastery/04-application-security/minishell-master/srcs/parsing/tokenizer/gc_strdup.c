/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:10:36 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/10 16:22:56 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*gc_strdup(t_data *data, const char *s)
{
	char	*dup;

	dup = ft_strdup(s);
	if (!dup)
		return (NULL);
	insert_malloc(data->hot_zone, dup);
	return (dup);
}
