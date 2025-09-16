/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 11:50:37 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/27 14:53:14 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*extract_key_from_assignment(const char *assignment, t_data *data)
{
	int		i;
	char	*key;

	i = 0;
	while (assignment[i] && assignment[i] != '=')
		i++;
	key = ft_calloc(sizeof(char), (i + 1));
	if (!key)
		return (NULL);
	ft_memcpy(key, assignment, i);
	key[i] = '\0';
	insert_malloc(data->cold_zone, key);
	return (key);
}

static char	*extract_value_from_assignment(const char *assignment)
{
	char	*equals_pos;

	equals_pos = ft_strchr(assignment, '=');
	if (!equals_pos)
		return (NULL);
	return (equals_pos + 1);
}

static int	process_export_assignment(const char *arg, t_data *data)
{
	char	*key;
	char	*value;

	key = extract_key_from_assignment(arg, data);
	if (!key || !is_valid_env_name(key))
	{
		ft_dprintf(2, "export: `%s': not a valid identifier\n", arg);
		return (1);
	}
	value = extract_value_from_assignment(arg);
	if (!value)
		value = "";
	if (add_env_var(data, key, value) == -1)
	{
		ft_dprintf(2, "export: `%s': not a valid identifier\n", arg);
		return (1);
	}
	return (0);
}

static int	handle_export_no_args(t_data *data, int write_to)
{
	print_env_sorted(data->envp_h, write_to);
	return (0);
}

int	builtin_export(char **args, t_data *data, int write_to)
{
	int	i;
	int	exit_status;

	if (!args || !args[1])
		return (handle_export_no_args(data, write_to));
	exit_status = 0;
	i = 1;
	while (args[i])
	{
		if (ft_strchr(args[i], '='))
		{
			if (process_export_assignment(args[i], data) == 1)
				exit_status = 1;
		}
		else if (!is_valid_env_name(args[i]))
			exit_status = ft_abs(ft_dprintf(2, "export: `%s': not a valid\
				identifier\n", args[i]) % 1);
		else
			if (find_env_var(data->envp_h, args[i]) == NULL
				&& create_new_env_node(data, args[i], "") == -1)
				exit_status = 1;
		i++;
	}
	return (exit_status);
}
