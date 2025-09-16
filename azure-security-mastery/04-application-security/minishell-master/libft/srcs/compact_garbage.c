/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   compact_garbage.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:18:55 by tritter           #+#    #+#             */
/*   Updated: 2025/05/29 15:18:51 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "garbage.h"

int	ft_create_add_arena(t_arena_g **lst, t_zone_g **zone)
{
	t_arena_g	*new_node;

	new_node = ft_new_arena_node(zone);
	if (!new_node)
		return (1);
	ft_arena_add(lst, new_node);
	return (0);
}

//Ajoute une valeur Malloc a une zone du garbage collector
void	insert_malloc(t_zone_g **garbage, void *malloc_value)
{
	t_zone_g	*temp;

	temp = ft_new_zone(malloc_value);
	if (!temp)
		return ;
	ft_zone_add_back(garbage, temp);
}
