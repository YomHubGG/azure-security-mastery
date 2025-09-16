/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 15:50:10 by tritter           #+#    #+#             */
/*   Updated: 2025/05/29 15:16:06 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GARBAGE_H
# define GARBAGE_H

//# include "./libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_zone_g
{
	void				*data;
	struct s_zone_g		*next;

}						t_zone_g;

typedef struct s_arena_g
{
	t_zone_g			**zone;
	struct s_arena_g	*next;

}						t_arena_g;

// garbage.c
void					*g_malloc(t_zone_g **garbage, size_t size);
t_zone_g				*ft_new_zone(void *content);
void					ft_zone_add_back(t_zone_g **lst, t_zone_g *new);
t_arena_g				*ft_new_arena_node(t_zone_g **zone);
void					ft_arena_add(t_arena_g **lst, t_arena_g *new);

// garbage_clean.c
void					g_zone_clean(t_zone_g **garbage);
void					g_arena_clean(t_arena_g *arena);

// g_calloc.c
void					*ft_g_calloc(t_zone_g *zone, size_t nmemb, size_t size);

// compact_garbage.c
char					**ft_g_split(t_zone_g **zone, char const *s, char c);
int						ft_create_add_arena(t_arena_g **lst, t_zone_g **zone);
void					insert_malloc(t_zone_g **garbage, void *malloc_value);

#endif