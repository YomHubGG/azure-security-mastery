/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helper2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:28:46 by tritter           #+#    #+#             */
/*   Updated: 2025/07/17 14:28:55 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	expand_word_helper_subwrp(t_expand_word *ac, t_data *data)
{
	(*ac).inbetween = ft_strdup("$");
	if ((*ac).inbetween)
		insert_malloc(data->hot_zone, (*ac).inbetween);
	(*ac).i2 = (*ac).fdtok + 1;
}
