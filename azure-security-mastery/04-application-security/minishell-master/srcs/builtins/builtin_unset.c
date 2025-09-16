/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ggrisole <ggrisole@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 19:44:12 by ggrisole          #+#    #+#             */
/*   Updated: 2025/06/30 15:10:28 by ggrisole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	validate_unset_arg(const char *arg)
{
	if (!arg || !*arg)
	{
		ft_dprintf(2, "unset: `': not a valid identifier\n");
		return (0);
	}
	if (!is_valid_env_name(arg))
	{
		ft_dprintf(2, "unset: `%s': not a valid identifier\n", arg);
		return (0);
	}
	return (1);
}

int	builtin_unset(char **args, t_data *data)
{
	int	i;
	int	exit_status;

	if (!args || !args[1])
		return (0);
	exit_status = 0;
	i = 1;
	while (args[i])
	{
		if (!validate_unset_arg(args[i]))
			exit_status = 1;
		else
			remove_env_var(data, args[i]);
		i++;
	}
	return (exit_status);
}
