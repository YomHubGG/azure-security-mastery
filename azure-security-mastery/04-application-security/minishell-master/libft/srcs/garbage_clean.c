/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_clean.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 10:01:13 by tritter           #+#    #+#             */
/*   Updated: 2025/05/29 15:19:12 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/garbage.h"

void	g_zone_clean(t_zone_g **garbage)
{
	t_zone_g	*temp;
	t_zone_g	*next;

	if (!garbage || !*garbage)
		return ;
	temp = *garbage;
	while (temp)
	{
		next = temp->next;
		free(temp->data);
		free(temp);
		temp = next;
	}
	*garbage = NULL;
}

void	g_arena_clean(t_arena_g *arena)
{
	t_arena_g	*temp;
	t_arena_g	*next;

	temp = arena;
	while (temp)
	{
		next = temp->next;
		if (temp->zone)
		{
			g_zone_clean(temp->zone);
			free(temp->zone);
		}
		free(temp);
		temp = next;
	}
}
