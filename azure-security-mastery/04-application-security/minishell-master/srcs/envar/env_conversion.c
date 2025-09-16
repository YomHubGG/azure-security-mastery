/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_conversion.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 14:40:27 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/27 12:44:06 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

// envp to array conversion for my team-sloth 🦥

void	pes_helper(int write_to, char **env_array, int i, char *equals_pos)
{
	write(write_to, env_array[i], equals_pos - env_array[i]);
	write(write_to, "=\"", 2);
	write(write_to, equals_pos + 1, ft_strlen(equals_pos + 1));
	write(write_to, "\"", 1);
}

static int	count_env_nodes(t_envp *head)
{
	int	count;

	count = 0;
	while (head)
	{
		count++;
		head = head->next;
	}
	return (count);
}

static char	**allocate_env_array(t_data *data, int count)
{
	char	**array;

	array = g_malloc(data->hot_zone, sizeof(char *) * (count + 1));
	if (!array)
		return (NULL);
	array[count] = NULL;
	return (array);
}

static void	fill_env_array(char **array, t_envp *head)
{
	int	i;

	i = 0;
	while (head)
	{
		array[i] = head->full_str;
		head = head->next;
		i++;
	}
}

char	**envp_list_to_array(t_data *data)
{
	char	**env_array;
	int		count;

	if (!data || !data->envp_h)
		return (NULL);
	count = count_env_nodes(data->envp_h);
	env_array = allocate_env_array(data, count);
	if (!env_array)
		return (NULL);
	fill_env_array(env_array, data->envp_h);
	return (env_array);
}
