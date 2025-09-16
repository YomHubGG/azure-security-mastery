/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tritter <tritter@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 12:02:16 by tritter           #+#    #+#             */
/*   Updated: 2025/07/31 08:22:26 by tritter          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	main(int argc, char **argv, char **envp)
{
	t_data	*data;

	(void)argc;
	(void)argv;
	data = init_data();
	if (!data)
		return (1);
	if (init_trash(data))
		return (1);
	if (init_syntax_error(data))
		return (1);
	if (!ft_fill_envp(data->cold_zone, envp, data))
		return (1);
	ensure_critical_env_vars(data);
	ft_signal_handler();
	exec_loop(data);
	g_arena_clean(data->g_arena);
	return (0);
}

t_data	*init_data(void)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
		return (NULL);
	data->last_exit_status = 0;
	data->envp_h = NULL;
	data->g_arena = NULL;
	data->tree_root = NULL;
	data->pid_list = NULL;
	data->pipe_list = NULL;
	data->redir_list = NULL;
	data->can_exec = true;
	data->is_piped = false;
	return (data);
}

int	init_trash(t_data *data)
{
	t_zone_g	**cold_zone;
	t_zone_g	**hot_zone;

	cold_zone = malloc(sizeof(t_zone_g *));
	if (!cold_zone)
		return (1);
	*cold_zone = ft_new_zone(NULL);
	if (!*cold_zone)
		return (free(cold_zone), 1);
	hot_zone = malloc(sizeof(t_zone_g *));
	if (!hot_zone)
		return (1);
	*hot_zone = ft_new_zone(NULL);
	if (!*hot_zone)
		return (free(hot_zone), 1);
	data->cold_zone = cold_zone;
	data->hot_zone = hot_zone;
	ft_create_add_arena(&data->g_arena, cold_zone);
	ft_create_add_arena(&data->g_arena, hot_zone);
	insert_malloc(data->cold_zone, data);
	return (0);
}

int	init_syntax_error(t_data *data)
{
	t_syntax_error	*err;

	err = g_malloc(data->cold_zone, sizeof(t_syntax_error));
	if (!err)
		return (1);
	err->has_error = false;
	err->message = NULL;
	err->token = NULL;
	data->set_syntax_error = err;
	return (0);
}
