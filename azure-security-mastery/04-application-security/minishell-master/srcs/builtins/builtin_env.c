/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 18:15:04 by ggrisole          #+#    #+#             */
/*   Updated: 2025/07/06 16:40:29 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_env(t_data *data, char **argv, int write_to)
{
	if (argv && argv[1])
	{
		ft_dprintf(2, "env: options not supported\n");
		return (127);
	}
	if (!data || !data->envp_h)
		return (1);
	print_env_unsorted(data->envp_h, write_to);
	return (0);
}
