/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:49:11 by tritter           #+#    #+#             */
/*   Updated: 2025/05/29 15:17:58 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/garbage.h"

//Stocke la valeur de retour dans la zone "t_zone_g garbage"
//prend la taille a malloc, retourne NULL en cas d'erreur
void	*g_malloc(t_zone_g **garbage, size_t size)
{
	void		*malloc_value;
	t_zone_g	*temp;

	temp = NULL;
	malloc_value = NULL;
	malloc_value = malloc(size);
	if (!malloc_value)
		return (NULL);
	temp = ft_new_zone(malloc_value);
	if (!temp)
		return (free(malloc_value), NULL);
	ft_zone_add_back(garbage, temp);
	return (malloc_value);
}

t_zone_g	*ft_new_zone(void *content)
{
	t_zone_g	*teliste;

	teliste = malloc(sizeof(t_zone_g));
	if (!teliste)
		return (NULL);
	teliste->data = content;
	teliste->next = NULL;
	return (teliste);
}

void	ft_zone_add_back(t_zone_g **lst, t_zone_g *new)
{
	t_zone_g	*temp;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	temp = *lst;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = new;
}

t_arena_g	*ft_new_arena_node(t_zone_g **zone)
{
	t_arena_g	*arena;

	arena = malloc(sizeof(t_arena_g));
	if (!arena)
		return (NULL);
	arena->zone = zone;
	arena->next = NULL;
	return (arena);
}

void	ft_arena_add(t_arena_g **lst, t_arena_g *new)
{
	t_arena_g	*temp;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	temp = *lst;
	while (temp->next != NULL)
		temp = temp->next;
	temp->next = new;
}
