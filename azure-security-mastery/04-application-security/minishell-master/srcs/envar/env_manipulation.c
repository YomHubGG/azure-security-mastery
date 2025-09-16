/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_manipulation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 15:23:08 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/15 10:06:43 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	update_existing_env(t_envp *node, const char *key, const char *value,
		t_data *data)
{
	char	*new_value;
	char	*new_full_str;

	new_value = ft_strdup(value);
	if (!new_value)
		return (-1);
	insert_malloc(data->cold_zone, new_value);
	new_full_str = create_env_string(data->cold_zone, (char *)key, new_value);
	if (!new_full_str)
		return (-1);
	node->value = new_value;
	node->full_str = new_full_str;
	return (0);
}

int	create_new_env_node(t_data *data, const char *key, const char *value)
{
	t_envp	*new_node;
	char	*new_key;
	char	*new_value;
	char	*new_full_str;

	new_key = ft_strdup(key);
	new_value = ft_strdup(value);
	if (!new_key || !new_value)
		return (-1);
	insert_malloc(data->cold_zone, new_key);
	insert_malloc(data->cold_zone, new_value);
	new_full_str = create_env_string(data->cold_zone, new_key, new_value);
	if (!new_full_str)
		return (-1);
	new_node = ft_new_env_node(data->cold_zone, new_key, new_value,
			new_full_str);
	if (!new_node)
		return (-1);
	ft_env_add_back(&data->envp_h, new_node);
	return (0);
}

int	update_env_var(t_data *data, const char *key, const char *value)
{
	t_envp	*node;

	if (!is_valid_env_name(key))
		return (-1);
	node = find_env_var(data->envp_h, key);
	if (!node)
		return (create_new_env_node(data, key, value));
	return (update_existing_env(node, key, value, data));
}

int	add_env_var(t_data *data, const char *key, const char *value)
{
	if (!is_valid_env_name(key))
		return (-1);
	if (find_env_var(data->envp_h, key))
		return (update_env_var(data, key, value));
	return (create_new_env_node(data, key, value));
}

int	remove_env_var(t_data *data, const char *key)
{
	t_envp	*current;
	t_envp	*prev;

	if (!key)
		return (-1);
	current = data->envp_h;
	prev = NULL;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			if (prev)
				prev->next = current->next;
			else
				data->envp_h = current->next;
			return (0);
		}
		prev = current;
		current = current->next;
	}
	return (-1);
}
