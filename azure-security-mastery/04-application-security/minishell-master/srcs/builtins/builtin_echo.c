/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 15:06:23 by ggrisole          #+#    #+#             */
/*   Updated: 2025/08/05 16:00:10 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static int	is_n_flag(const char *arg)
{
	int	i;

	if (!arg || arg[0] != '-')
		return (0);
	if (arg[1] != 'n')
		return (0);
	i = 2;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

int	builtin_echo(char **args, int write_to)
{
	int	i;
	int	newline;

	newline = 1;
	i = 1;
	if (!args)
	{
		write(write_to, "\n", 1);
		return (0);
	}
	while (args[i] && is_n_flag(args[i]))
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		write(write_to, args[i], ft_strlen(args[i]));
		if (args[i + 1])
			write(write_to, " ", 1);
		i++;
	}
	if (newline)
		write(write_to, "\n", 1);
	return (0);
}
