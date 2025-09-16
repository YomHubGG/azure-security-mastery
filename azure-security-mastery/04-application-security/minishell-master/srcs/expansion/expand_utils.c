/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/17 14:48:51 by tritter           #+#    #+#             */
/*   Updated: 2025/07/17 14:49:05 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//compares all keys to the provided key, 
//if none are found , an empty string is returned
char	*extract_word(char *key, t_data *data)
{
	t_envp	*current;

	current = data->envp_h;
	while (current)
	{
		if (ft_strcmp(key, current->key) == 0)
			return (current->value);
		current = current->next;
	}
	return ("");
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	if (!s1 && !s2)
		return (0);
	if (!s1)
		return (-1);
	if (!s2)
		return (1);
	i = 0;
	while (s1[i] && s2[i])
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}
