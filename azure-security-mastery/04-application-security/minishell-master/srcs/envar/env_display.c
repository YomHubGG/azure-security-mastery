/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_display.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 14:42:50 by ggrisole          #+#    #+#             */
/*   Updated: 2025/08/05 12:03:51 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	print_env_unsorted(t_envp *head, int write_to)
{
	while (head)
	{
		if (head->value && head->value[0] != '\0')
		{
			write(write_to, head->full_str, ft_strlen(head->full_str));
			write(write_to, "\n", 1);
		}
		head = head->next;
	}
}

int	count_env_vars(t_envp *head)
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

void	sort_env_array(char **array, int size)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < size - 1)
	{
		j = 0;
		while (j < size - 1 - i)
		{
			if (ft_strcmp(array[j], array[j + 1]) > 0)
			{
				temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

char	**create_env_array(t_envp *head, int count)
{
	char	**env_array;
	int		i;

	env_array = malloc(sizeof(char *) * count);
	if (!env_array)
		return (NULL);
	i = 0;
	while (head && i < count)
	{
		env_array[i] = head->full_str;
		head = head->next;
		i++;
	}
	return (env_array);
}

void	print_env_sorted(t_envp *head, int write_to)
{
	char	**env_array;
	int		count;
	int		i;
	char	*equals_pos;

	count = count_env_vars(head);
	if (count == 0)
		return ;
	env_array = create_env_array(head, count);
	if (!env_array)
		return ;
	sort_env_array(env_array, count);
	i = 0;
	while (i < count)
	{
		equals_pos = ft_strchr(env_array[i], '=');
		write(write_to, "declare -x ", ft_strlen("declare -x "));
		if (equals_pos)
			pes_helper(write_to, env_array, i, equals_pos);
		else
			write(write_to, env_array[i], ft_strlen(env_array[i]));
		write(write_to, "\n", 1);
		i++;
	}
	free(env_array);
}
