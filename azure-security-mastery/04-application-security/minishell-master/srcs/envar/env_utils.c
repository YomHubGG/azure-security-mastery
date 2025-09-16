/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 14:42:50 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/27 10:33:01 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_envp	*find_env_var(t_envp *head, const char *key)
{
	if (!key)
		return (NULL);
	while (head)
	{
		if (ft_strcmp(head->key, key) == 0)
			return (head);
		head = head->next;
	}
	return (NULL);
}

char	*get_env_value(t_envp *head, const char *key)
{
	t_envp	*node;

	node = find_env_var(head, key);
	if (node)
		return (node->value);
	return (NULL);
}

int	is_valid_env_name(const char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

char	*create_env_string(t_zone_g **zone, char *key, char *value)
{
	char	*key_equals;
	char	*result;

	key_equals = ft_strjoin_injector(key, "=", zone);
	if (!key_equals)
		return (NULL);
	result = ft_strjoin_injector(key_equals, value, zone);
	return (result);
}
